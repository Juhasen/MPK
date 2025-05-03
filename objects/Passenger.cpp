#include <Ice/Ice.h>
#include <SIPI.h>

void
SIP::PassengerI::updateTramInfo(::std::shared_ptr<TramPrx> /*tram*/,
                                StopList /*stops*/,
                                const Ice::Current& current)
{
}

void
SIP::PassengerI::updateStopInfo(::std::shared_ptr<StopPrx> /*stop*/,
                                TramList /*trams*/,
                                const Ice::Current& current)
{
}
