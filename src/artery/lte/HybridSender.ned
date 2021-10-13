// 
//                           SimuLTE
// 
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself, 
// and cannot be removed from it.
// 

package artery.lte;

import inet.applications.contract.IUDPApp;


simple HybridSender like IUDPApp
{
    parameters:
        int localPort = default(3000);
        int destPort = default(3001);
        int PacketSize = default(40);

        @display("i=block/source");
    gates:
        output udpOut;
        input udpIn;
}
