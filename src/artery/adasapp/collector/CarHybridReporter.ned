package artery.adasapp.collector;

module CarHybridReporter
{
    parameters:
        string centralAddress;
        int centralPort = default(3000);
        @class(adasapp::CarHybridReporter);

    gates:
        inout radioDriverGate;
        inout wlanGate;
        inout lteGate;
}
