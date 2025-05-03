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
    return stops_;
}

void
LineI::registerTram(const TramPrx& tram,
                         const Ice::Current& current)
{
    trams_.push_back(tram);
}

void
LineI::unregisterTram(const TramPrx& tram,
                           const Ice::Current& current)
{
    auto it = std::remove(trams_.begin(), trams_.end(), tram);
    if (it != trams_.end()) {
        trams_.erase(it, trams_.end());
    }
    cout << "Tram unregistered." << endl;
}

void
LineI::setStops(const StopList& sl,
                     const Ice::Current& current)
{
    stops_ = sl;
}

string
LineI::getName(const Ice::Current& current)
{
    return name_;
}
