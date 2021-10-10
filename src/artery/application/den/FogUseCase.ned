package artery.application.den;

simple FogUseCase like UseCase
{
    @class(den::FogUseCase);
    double detectionBlockingInterval @unit(s) = default(20s);
}
