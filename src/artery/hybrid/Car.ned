package artery.hybrid;

import artery.hybrid.collector.CarHybridReporter;

module Car extends artery.lte.Car
{
    parameters:
        @display("r=1000");

    submodules:
        hybridReporter: CarHybridReporter {
            @display("p=250,300");
        }

    connections:
        for i = 0..numUdpApps-1 {
            udpApp[i].geoNetOut --> hybridReporter.lteGate$i;
        }

        for i=0..sizeof(radioIn)-1 {
            hybridReporter.wlanGate$i <-- {@reconnect;} <-- wlan[i].upperLayerOut;
            hybridReporter.wlanGate$o --> {@reconnect;} --> wlan[i].upperLayerIn;
            
            hybridReporter.radioDriverGate$i <-- {@reconnect;} <-- radioDriver[i].lowerLayerOut;
            hybridReporter.radioDriverGate$o --> {@reconnect;} --> radioDriver[i].lowerLayerIn;
            
            hybridReporter.lteGate$o --> networkLayer.transportIn++;
        }
}