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
TramStopI::UpdateTramInfo(const TramPrx &tram,
                          const Time &time,
                          const Ice::Current &current) {
    TramList next_trams;
    TramPrxList trams = tram->getLine()->getTrams();
    for (TramPrx &tram: trams) {
        StopList schedule = tram->getSchedule();
        for (StopInfo &info: schedule) {
            if (info.stop->getName() == name_) {
                if ((info.time.hour > time.hour) ||
                    (info.time.hour == time.hour && info.time.minute >= time.minute)) {
                    TramInfo tram_info;
                    tram_info.tram = tram;
                    tram_info.time = info.time;
                    next_trams.push_back(tram_info);
                }
            }
        }
    }
    //sortowanie po czasie
    std::sort(next_trams.begin(), next_trams.end(),
              [](const TramInfo &a, const TramInfo &b) {
                  return (a.time.hour < b.time.hour) ||
                         (a.time.hour == b.time.hour && a.time.minute < b.time.minute);
              });

    try {
        TramStopPrx currentTramStop = tram->getLocation();
        for (const auto &passenger: passengers_) {
            cout << "Updating passenger info on "
     << currentTramStop->getName()
     << "..." << endl;
            cout<<passengers_.size()<<endl;
            passenger->updateStopInfo(currentTramStop, next_trams);
            cout << "Passenger info updated." + currentTramStop->getName() << endl;
        }
    } catch (const Ice::Exception &e) {
        cerr << e << endl;
    }


}
