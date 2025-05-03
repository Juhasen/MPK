#include <Ice/Ice.h>
#include "SIPI.h"

string
TramStopI::getName(const Ice::Current& current)
{
    return name_;
}

TramList
TramStopI::getNextTrams(int howMany,
                             const Ice::Current& current)
{
    if (howMany <= 0) {
        return TramList();
    }
    TramList result;
    for (int i = 0; i < howMany && i < next_trams_.size(); ++i) {
        result.push_back(next_trams_[i]);
    }
    return result;
}

void
TramStopI::RegisterPassenger(const PassengerPrx& passenger,
                                  const Ice::Current& current)
{
    passengers_.push_back(passenger);
}

void
TramStopI::UnregisterPassenger(const PassengerPrx& passenger,
                                    const Ice::Current& current)
{
    auto it = std::remove(passengers_.begin(), passengers_.end(), passenger);
    if (it != passengers_.end()) {
        passengers_.erase(it, passengers_.end());
    }
    cout << "Passenger unregistered." << endl;
}

void
TramStopI::UpdateTramInfo(const TramPrx& tram,
                               const Time& time,
                               const Ice::Current& current)
{
    StopList stop_list = tram->getNextStops(5);

    for (const auto& passenger : passengers_) {
        passenger->updateTramInfo(tram, stop_list);
    }

    cout << "Tram info updated." << endl;
}

