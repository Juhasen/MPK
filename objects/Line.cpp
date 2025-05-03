#include <Ice/Ice.h>
#include "SIPI.h"

TramList
LineI::getTrams(const Ice::Current& current)
{
    return TramList();
}

StopList
LineI::getStops(const Ice::Current& current)
{
    return StopList();
}

void
LineI::registerTram(const TramPrx& tram,
                         const Ice::Current& current)
{
}

void
LineI::unregisterTram(const TramPrx& tram,
                           const Ice::Current& current)
{
}

void
LineI::setStops(const StopList& sl,
                     const Ice::Current& current)
{
}

string
LineI::getName(const Ice::Current& current)
{
    return name_;
}
