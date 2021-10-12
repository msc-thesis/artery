package artery.adasapp.collector;

module HybridReporter
{
    parameters:
        int centralPort = default(9320);
        string centralAddress;
        @class(adasapp::HybridReporter);

    gates:
        input socketIn;
        output wlanOut;
        output ethOut;
        input socketIn2;
        output socketOut2;
        input socketIn3;
}
