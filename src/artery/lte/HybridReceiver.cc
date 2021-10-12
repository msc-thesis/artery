//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "artery/lte/HybridReceiver.h"
#include "artery/networking/GeoNetPacket.h"

namespace artery
{

Define_Module(HybridReceiver);

void HybridReceiver::initialize(int stage)
{
    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;

    int port = par("localPort");
    geoNetOut = gate("geoNetOut");

    EV << "HybridReceiver::initialize - binding to port: local:" << port << endl;
    if (port != -1)
    {
        socket.setOutputGate(gate("udpOut"));
        socket.bind(port);
    }
}

void HybridReceiver::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        return;

    send(msg, geoNetOut);
}

}
