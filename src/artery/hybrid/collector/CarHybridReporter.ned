package artery.hybrid.collector;

module CarHybridReporter
{
    parameters:
        string centralAddress;
        int centralPort = default(3000);
        @class(hybrid::CarHybridReporter);

    gates:
        inout radioDriverGate;
        inout wlanGate;
        inout lteGate;
}
