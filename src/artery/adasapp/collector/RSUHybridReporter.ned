package artery.adasapp.collector;

module RSUHybridReporter
{
    parameters:
        string centralAddress;
        int centralPort = default(3000);
        @class(adasapp::RSUHybridReporter);

    gates:
        inout radioDriverGate;
        inout wlanGate;
        output ethOut;
}
