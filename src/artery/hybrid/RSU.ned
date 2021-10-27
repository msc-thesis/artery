package artery.hybrid;

import inet.linklayer.contract.IWiredNic;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.contract.INetworkLayer;
import artery.hybrid.collector.RSUHybridReporter;

module RSU extends artery.inet.RSU
{
    parameters:
        string networkLayerType = default("IPv4NetworkLayer");
        networklayer.proxyARP = default(false);
        string routingTableType = default("IPv4RoutingTable");
        *.routingTableModule = default(routingTableType != "" ? absPath(".routingTable") : "");
    
    submodules:
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
            hybridReporter.wlanGate$i <-- {@reconnect;} <-- wlan[i].upperLayerOut;
            radioDriver[i].lowerLayerIn <-- {@reconnect;} <-- hybridReporter.radioDriverGate$o;

            radioDriver[i].lowerLayerOut --> {@reconnect;} --> hybridReporter.radioDriverGate$i;
            hybridReporter.wlanGate$o --> {@reconnect;} --> wlan[i].upperLayerIn;

            hybridReporter.ethOut --> networkLayer.transportIn++;
        }

        for i=0..sizeof(ethg)-1 {
            ethg[i] <--> { @display("m=s"); } <--> eth[i].phys;
            eth[i].upperLayerOut --> networkLayer.ifIn++;
            eth[i].upperLayerIn <-- networkLayer.ifOut++;
        }
}