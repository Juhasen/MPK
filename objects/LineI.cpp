#include <Ice/Ice.h>
#include "SIPI.h"

TramPrxList
LineI::getTrams(const Ice::Current& current)
{
    return trams_;
}

TramStopList
LineI::getStops(const Ice::Current& current)
{
    return stops_;
}

TramStopPrx
LineI::getStop(const string& name,
                     const Ice::Current& current)
{
    for (const TramStopPrx& stop: stops_) {
        if (stop->getName() == name) {
            return stop;
        }
    }
    return nullptr;
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
    auto it = std::find(trams_.begin(), trams_.end(), tram);
    if (it != trams_.end()) {
        trams_.erase(it);
        cout << "Tram unregistered: " << tram->ice_toString() << endl;
    } else {
        cout << "Tram not found in the list." << endl;
    }
}

void
LineI::setStops(const TramStopList& sl,
                     const Ice::Current& current)
{
    stops_ = sl;
}

string
LineI::getName(const Ice::Current& current)
{
    return name_;
}
