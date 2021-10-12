package artery.adasapp.collector;

module RSUHybridReporter
{
    parameters:
        int centralPort = default(9320);
        string centralAddress;
        @class(adasapp::RSUHybridReporter);

    gates:
        input radioIn;
        output radioOut;
        input wlanIn;
        output wlanOut;
        output ethOut;
}
