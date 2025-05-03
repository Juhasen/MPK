#include <Ice/Ice.h>
#include <SIPI.h>

::std::shared_ptr<::SIP::TramStopPrx>
SIP::TramI::getLocation(const Ice::Current& current)
{
    return nullptr;
}

::std::shared_ptr<::SIP::LinePrx>
SIP::TramI::getLine(const Ice::Current& current)
{
    return nullptr;
}

void
SIP::TramI::setLine(::std::shared_ptr<LinePrx> /*line*/,
                    const Ice::Current& current)
{
}

::SIP::StopList
SIP::TramI::getNextStops(int /*howMany*/,
                         const Ice::Current& current)
{
    return StopList();
}

void
SIP::TramI::RegisterPassenger(::std::shared_ptr<PassengerPrx> /*p*/,
                              const Ice::Current& current)
{
}

void
SIP::TramI::UnregisterPassenger(::std::shared_ptr<PassengerPrx> /*p*/,
                                const Ice::Current& current)
{
}

::std::string
SIP::TramI::getStockNumber(const Ice::Current& current)
{
    return ::std::string();
}