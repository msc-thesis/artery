package artery.adasapp.collector;

module RSUHybridReporter
{
    parameters:
        int centralPort = default(9320);
        string centralAddress;
        @class(adasapp::RSUHybridReporter);

    gates:
        input socketIn;
        output wlanOut;
        output ethOut;
        input socketIn2;
        output socketOut2;
        input socketIn3;
}
