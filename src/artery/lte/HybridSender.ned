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
