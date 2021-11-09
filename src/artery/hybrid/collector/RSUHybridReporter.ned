package artery.hybrid.collector;

module RSUHybridReporter
{
    parameters:
        string centralAddress;
        int centralPort = default(3000);
        @class(hybrid::RSUHybridReporter);

    gates:
        inout radioDriverGate;
        inout wlanGate;
        output ethOut;
        input LTEIn;
}
