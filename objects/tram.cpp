#include <Ice/Ice.h>
#include "SIPI.h"

TramStopPrx
TramI::getLocation(const Ice::Current &current) {
    return location_;
}

void
TramI::setLocation(const TramStopPrx &location_prx, const Ice::Current &) {
    location_ = location_prx;
}

void
TramI::setDepo(const DepoPrx &depo,
               const Ice::Current &current) {
    depo_ = depo;
}

DepoPrx
TramI::getDepo(const Ice::Current &current) {
    return depo_;
}

LinePrx
TramI::getLine(const Ice::Current &current) {
    return line_;
}

void
TramI::setLine(const LinePrx &line,
               const Ice::Current &current) {
    line_ = line;
}

void
TramI::setTram(const TramPrx &tram,
               const Ice::Current &current) {
    tram_ = tram;
}

StopList
TramI::getNextStops(int howMany,
                    const Ice::Current &current) {
    if (howMany <= 0) {
        return StopList();
    }

    StopList stops = schedule_;
    unsigned int totalStops = stops.size();

    //get index of stop from current position
    int idx = 0;
    for (unsigned int i = 0; i < totalStops; ++i) {
        if (stops[i].stop->getName() == location_->getName()) {
            idx = i;
            break;
        }
    }

    //calculate next stops
    StopList result;
    for (int i = 1; i <= howMany; ++i) {
        unsigned int nextIdx = (idx + i) % totalStops;
        result.push_back(stops[nextIdx]);
    }

    return result;
}

void
TramI::RegisterPassenger(const PassengerPrx &passenger,
                         const Ice::Current &current) {
    passengers_.push_back(passenger);
    cout << "\nPassenger registered." << endl;
}

void
TramI::UnregisterPassenger(const PassengerPrx &passenger,
                           const Ice::Current &current) {
    auto it = std::remove(passengers_.begin(), passengers_.end(), passenger);
    if (it != passengers_.end()) {
        passengers_.erase(it, passengers_.end());
    }
    cout << "\nPassenger unregistered." << endl;
}

void
TramI::updatePassengerInfo(const TramPrx &tram, const Ice::Current &current) {
    StopList stop_list = getNextStops(3, current);
    for (const auto &passenger: passengers_) {
        passenger->updateTramInfo(tram, stop_list);
    }
}

string
TramI::getStockNumber(const Ice::Current &current) {
    return stock_number_;
}

void
TramI::setSchedule(const StopList &schedule,
                   const Ice::Current &current) {
    schedule_ = schedule;
}

StopList
TramI::getSchedule(const Ice::Current &current) {
    return schedule_;
}

void
TramI::updateSchedule(int interval,
                      const Ice::Current &current) {
    // 1. Pobierz stary rozkład
    StopList oldSchedule = tram_->getSchedule();
    const int minutesInDay = 24 * 60;

    // 2. Wyznacz czas startu na podstawie ostatniego przystanku
    int startTotal = 0;
    if (!oldSchedule.empty()) {
        auto tLast = oldSchedule.back().time;
        startTotal = tLast.hour * 60 + tLast.minute + interval;
        startTotal %= minutesInDay;
        if (startTotal < 0) startTotal += minutesInDay;
    }
    // Jeśli stary rozkład był pusty, zostanie 00:00

    // 3. Pobierz listę przystanków linii
    auto tramStops = tram_->getLine()->getStops();
    StopList newSchedule;
    newSchedule.reserve(tramStops.size());

    // 4. Generuj nowy rozkład: pierwszy od startTotal, kolejne co interval
    for (size_t i = 0; i < tramStops.size(); ++i) {
        int t = (startTotal + interval * static_cast<int>(i)) % minutesInDay;
        if (t < 0) t += minutesInDay;

        StopInfo info;
        info.stop = tramStops[i];
        info.time.hour = t / 60;
        info.time.minute = t % 60;
        newSchedule.push_back(info);
    }

    // 5. Ustaw nowy rozkład w obiekcie tramwaju
    tram_->setSchedule(newSchedule);

    cout << "Schedule rotated: first stop at last_old_time+interval." << endl;
}

int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter tram configuration (tram_stock_number/port): ";
    string input;
    cin >> input;
    size_t first_slash = input.find('/');
    string tram_stock_number = input.substr(0, first_slash);
    string port = input.substr(first_slash + 1);
    string host = getNetworkInterface();
    cout << "Creating tram: " << tram_stock_number << " on host " + host + " on port " + port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("TramAdapter", "tcp -h " + host + " -p " + port);
        Ice::ObjectPtr object = new TramI(tram_stock_number);
        adapter->add(object, Ice::stringToIdentity(tram_stock_number));
        adapter->activate();

        cout << "Tram object created." << endl;

        TramPrx tram = Ice::uncheckedCast<TramPrx>(adapter->createProxy(Ice::stringToIdentity(tram_stock_number)));
        if (!tram) {
            throw "Invalid proxy";
        }

        tram->setTram(tram);

        while (true) {
            cout << "\nMENU:" << endl;
            cout << "1. Show stops." << endl;
            cout << "n. Go to next location." << endl;
            cout << "q. Quit." << endl;
            char choice = '0';
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 'q') {
                break;
            }
            switch (choice) {
                case '1': {
                    if (!tram->getLocation()) {
                        cout << "Tram is not assigned to any line." << endl;
                        break;
                    }
                    cout << "Current position: " << tram->getLocation()->getName() << endl;
                    cout << "Assigned stops:" << endl;
                    TramStopList stops = tram->getLine()->getStops();
                    if (stops.size() == 0 || !stops.data()) {
                        cout << "No stops found." << endl;
                        break;
                    }
                    for (TramStopPrx stop: stops) {
                        cout << "Stop: " << stop->getName() << endl;
                    }
                    break;
                }
                case 'n': {
                    if (!tram->getLocation()) {
                        cout << "Tram is not assigned to any line." << endl;
                        break;
                    }
                    string previous = tram->getLocation()->getName();
                    cout << "Previous position: " << previous << endl;
                    tram->setLocation(tram->getNextStops(1)[0].stop);
                    cout << "New position: " << tram->getLocation()->getName() << endl;

                    TramStopPrx currentStop = tram->getLocation();
                    if (currentStop->getName() == tram->getLine()->getStops()[0]->getName()) {
                        tram->updateSchedule(7);
                    }
                    tram->updatePassengerInfo(tram);

                    if (currentStop) {
                        currentStop->updatePassengerInfo(tram);
                    }
                    break;
                }
                default: {
                    cout << "Unknown command." << endl;
                    break;
                }
            }
        }

        ic->waitForShutdown();
    } catch (const Ice::Exception &e) {
        cerr << e << endl;
        status = 1;
    } catch (const char *msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->shutdown();
            ic->destroy();
        } catch (const Ice::Exception &e) {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}
