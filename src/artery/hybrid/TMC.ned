package artery.hybrid;

import inet.node.inet.StandardHost;
import artery.networking.Vanetza;
import artery.application.StationaryMiddleware;


module TMC extends StandardHost
{
    parameters:
        @display("p=1500,60;i=device/pc");
        numRadios = default(0);
}
