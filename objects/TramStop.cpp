#include <Ice/Ice.h>
#include <SIPI.h>

string
TramStopI::getName(const Ice::Current& current)
{
    return string();
}

TramList
TramStopI::getNextTrams(int /*howMany*/,
                             const Ice::Current& current)
{
    return TramList();
}

void
TramStopI::RegisterPassenger(shared_ptr<PassengerPrx> /*p*/,
                                  const Ice::Current& current)
{
}

void
TramStopI::UnregisterPassenger(shared_ptr<PassengerPrx> /*p*/,
                                    const Ice::Current& current)
{
}

void
TramStopI::UpdateTramInfo(shared_ptr<TramPrx> /*tram*/,
                               Time /*time*/,
                               const Ice::Current& current)
{
}

