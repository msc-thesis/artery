package artery.hybrid;

import artery.application.StationaryMiddleware;
import artery.hybrid.Vanetza;
import artery.inet.RadioDriver;
import inet.node.inet.INetworkNode;
import inet.linklayer.contract.IWirelessNic;
import inet.mobility.contract.IMobility;
import inet.networklayer.common.InterfaceTable;
import inet.linklayer.contract.IWiredNic;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.contract.INetworkLayer;
import artery.hybrid.collector.RSUHybridReporter;

module RSU like INetworkNode
{
    parameters:
        @display("i=block/wrxtx;is=vs");
        @networkNode;
        @labels(node,ethernet-node,wireless-node);
        int numRadios = default(1);
        bool withAntennaMobility = default(false);
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        mobility.visualRepresentation = "^";
        string networkLayerType = default("IPv4NetworkLayer");
        networklayer.proxyARP = default(false);
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

        mobility: <default("inet.mobility.static.StationaryMobility")> like IMobility {
            parameters:
                @display("p=53,200");
        }

        middleware: StationaryMiddleware {
            parameters:
                @display("p=50,100");
        }
        
        networkLayer: <networkLayerType> like INetworkLayer {
            parameters:
                @display("p=368,287;q=queue");
        }

        routingTable: <routingTableType> like IRoutingTable if routingTableType != "" {
            parameters:
                @display("p=53,250;is=s");
        }

        eth[sizeof(ethg)]: <default("EthernetInterface")> like IWiredNic {
            parameters:
                @display("p=368,406,row,60;q=txQueue");
        }

        hybridReporter: RSUHybridReporter {
            parameters:
                @display("p=280,287");
        }

    connections allowunconnected:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wlan[i].radioIn;

            hybridReporter.wlanGate$i <-- {@reconnect;} <-- wlan[i].upperLayerOut;
            radioDriver[i].lowerLayerIn <-- {@reconnect;} <-- hybridReporter.radioDriverGate$o;

            radioDriver[i].lowerLayerOut --> {@reconnect;} --> hybridReporter.radioDriverGate$i;
            hybridReporter.wlanGate$o --> {@reconnect;} --> wlan[i].upperLayerIn;

            hybridReporter.ethOut --> networkLayer.transportIn++;

            radioDriver[i].upperLayer <--> vanetza[i].radioDriverData;
            radioDriver[i].properties --> vanetza[i].radioDriverProperties;
        }

        for i=0..sizeof(ethg)-1 {
            ethg[i] <--> { @display("m=s"); } <--> eth[i].phys;
            eth[i].upperLayerOut --> networkLayer.ifIn++;
            eth[i].upperLayerIn <-- networkLayer.ifOut++;
        }
}
