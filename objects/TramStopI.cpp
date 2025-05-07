#include <Ice/Ice.h>
#include "SIPI.h"

string
TramStopI::getName(const Ice::Current &current) {
    return name_;
}

void
TramStopI::setNextTrams(const TramList &next_trams,
                        const Ice::Current &current) {
    next_trams_ = next_trams;
}

TramList
TramStopI::getNextTrams(int howMany,
                        const Ice::Current &current) {
    if (howMany <= 0) {
        return {};
    }
    TramList result;
    for (int i = 0; i < howMany && i < next_trams_.size(); ++i) {
        result.push_back(next_trams_[i]);
    }
    return result;
}

void
TramStopI::RegisterPassenger(const PassengerPrx &passenger,
                             const Ice::Current &current) {
    if (std::find(passengers_.begin(), passengers_.end(), passenger) == passengers_.end()) {
        passengers_.push_back(passenger);
        cout << "\nPassenger registered." << endl;
    } else {
        cout << "\nPassenger is already registered." << endl;
    }
}

void
TramStopI::UnregisterPassenger(const PassengerPrx &passenger,
                               const Ice::Current &current) {
    auto it = std::remove(passengers_.begin(), passengers_.end(), passenger);
    if (it != passengers_.end()) {
        passengers_.erase(it, passengers_.end());
    }
    cout << "Passenger unregistered." << endl;
}


void
TramStopI::updatePassengerInfo(const TramPrx& tram, const Ice::Current &current) {
    for (const auto &passenger: passengers_) {
        passenger->updateTramStopInfo(tram);
    }
}