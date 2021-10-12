package artery.inet;

import artery.application.StationaryMiddleware;
import artery.networking.Vanetza;
import artery.nic.IRadioDriver;
import inet.node.inet.INetworkNode;
import inet.linklayer.contract.IWirelessNic;
import inet.linklayer.contract.IWiredNic;
import inet.mobility.contract.IMobility;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.contract.INetworkLayer;
import inet.networklayer.common.InterfaceTable;
import artery.adasapp.collector.RSUHybridReporter;

module RSU like INetworkNode
{
    parameters:
        @display("i=block/wrxtx;is=vs");
        @networkNode;
        @labels(node,ethernet-node,wireless-node);
        int numRadios = default(1);
        bool hybridScenario = default(false);
        string networkLayerType = default("IPv4NetworkLayer");
        bool withAntennaMobility = default(false);
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        mobility.visualRepresentation = "^";
        networkLayer.proxyARP = default(false);
        string routingTableType = default("IPv4RoutingTable");
        *.routingTableModule = default(routingTableType != "" ? absPath(".routingTable") : "");
    gates:
        input radioIn[numRadios] @directIn;
        inout pppg[] @labels(PPPFrame-conn);
        inout ethg[] @labels(EtherFrame-conn);

    submodules:
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=53,300;is=s");
        }

        wlan[numRadios]: <default("VanetNic")> like IWirelessNic {
            parameters:
                @display("p=280,406,row,60;q=queue");
                radio.antenna.mobilityModule = withAntennaMobility ?
                    ".antennaMobility[" + string(index) + "]" : ".mobility";
        }

        // network layer
        networkLayer: <networkLayerType> like INetworkLayer if hybridScenario {
            parameters:
                @display("p=368,287;q=queue");
        }

        // routing table
        routingTable: <routingTableType> like IRoutingTable if routingTableType != "" && hybridScenario {
            parameters:
                @display("p=53,250;is=s");
        }

        eth[sizeof(ethg)]: <default("EthernetInterface")> like IWiredNic if hybridScenario {
            parameters:
                @display("p=368,406,row,60;q=txQueue");
        }

        antennaMobility[numRadios]: AntennaMobility if withAntennaMobility == true {
            parameters:
                @display("p=100,200");
                mobilityModule = "^.mobility";
        }

        radioDriver[numRadios]: RadioDriver {
            parameters:
                @display("p=192,287");
                macModule = ".wlan[" + string(index) + "].mac";
                radioModule = ".wlan[" + string(index) + "].radio";
        }

        vanetza[numRadios]: Vanetza {
            parameters:
                @display("p=192,406");
                *.middlewareModule = absPath("^.middleware");
                *.mobilityModule = absPath("^.mobility");
                *.radioDriverModule = absPath("^.radioDriver[" + string(index) + "]");
                *.traciCoreModule = "traci.core";
                position.typename = default("StationaryPositionProvider");
                runtime.datetime = middleware.datetime;
                router.isMobile = false;
        }

        hybridReporter: RSUHybridReporter if hybridScenario {
            parameters:
                @display("p=280,287");
        }

        mobility: <default("inet.mobility.static.StationaryMobility")> like IMobility {
            parameters:
                @display("p=53,200");
        }

        middleware: StationaryMiddleware {
            parameters:
                @display("p=50,100");
        }

    connections allowunconnected:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wlan[i].radioIn;

            wlan[i].upperLayerOut --> radioDriver[i].lowerLayerIn if !hybridScenario;
            wlan[i].upperLayerIn <-- radioDriver[i].lowerLayerOut if !hybridScenario;

            hybridReporter.socketIn2 <-- wlan[i].upperLayerOut if hybridScenario;
            radioDriver[i].lowerLayerIn <-- hybridReporter.socketOut2 if hybridScenario;

            radioDriver[i].lowerLayerOut --> hybridReporter.socketIn if hybridScenario;
            hybridReporter.wlanOut --> wlan[i].upperLayerIn if hybridScenario;
            hybridReporter.ethOut --> networkLayer.transportIn++ if hybridScenario;

            radioDriver[i].upperLayer <--> vanetza[i].radioDriverData;
            radioDriver[i].properties --> vanetza[i].radioDriverProperties;
        }

        for i=0..sizeof(ethg)-1 {
            ethg[i] <--> { @display("m=s"); } <--> eth[i].phys if hybridScenario;
            eth[i].upperLayerOut --> networkLayer.ifIn++ if hybridScenario;
            eth[i].upperLayerIn <-- networkLayer.ifOut++ if hybridScenario;
        }
}
