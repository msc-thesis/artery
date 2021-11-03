package artery.hybrid;

import inet.node.ethernet.Eth10G;
import artery.hybrid.TMC;
import artery.hybrid.RSU;

network World extends artery.lte.World
{
    parameters:
        @display("bgb=15000,5000;bgg=1000,2,grey95;bgu=m");
        int numHybridRoadSideUnits = default(0);
    
    submodules:
        tmc: TMC {
            @display("p=500,4500");
        }

        rsu[numHybridRoadSideUnits]: RSU {
            parameters:
                mobility.initFromDisplayString = false;
        }
    
    connections:
        router.pppg++ <--> Eth10G <--> tmc.pppg++;
        
        for i = 0..numHybridRoadSideUnits-1 {
            rsu[i].ethg++ <--> Eth10G <--> tmc.ethg++;
        }
}
