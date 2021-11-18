package artery.lte;

import inet.applications.contract.IUDPApp;


simple HybridReceiver like IUDPApp
{
    parameters:
        int localPort = default(3000);
    gates:
        output udpOut;
        input udpIn;
        output geoNetOut;
}

