package artery.hybrid;

simple HybridFsmDccEntity extends artery.networking.FsmDccEntity
{
    parameters:
        @class(HybridFsmDccEntity);

    gates:
        output LTEOut;
}
