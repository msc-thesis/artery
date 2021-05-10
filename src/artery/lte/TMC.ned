package artery.lte;

import inet.node.inet.StandardHost;
import artery.networking.Vanetza;
import artery.application.StationaryMiddleware;


module TMC extends StandardHost
{
    parameters:
        @display("p=1500,60;i=device/pc");
        numRadios = default(0);


    submodules:
        // radioDriver[numRadios]: RadioDriver {
        //     parameters:
        //         macModule = ".wlan[" + string(index) + "].mac";
        //         radioModule = ".wlan[" + string(index) + "].radio";
        // }

        // vanetza[numRadios]: Vanetza {
        //     parameters:
        //         *.middlewareModule = absPath("^.middleware");
        //         *.mobilityModule = absPath("^.mobility");
        //         *.radioDriverModule = absPath("^.radioDriver[" + string(index) + "]");
        //         *.traciCoreModule = "traci.core";
        //         position.typename = default("StationaryPositionProvider");
        //         runtime.datetime = middleware.datetime;
        //         router.isMobile = false;
        // }

        // middleware: StationaryMiddleware {
        //     parameters:
        //         @display("p=50,100");
        // };


}
