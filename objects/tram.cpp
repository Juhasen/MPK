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
    //find and remove passenger
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
        cout << "\nTram info updated tram->updatePassengerInfo" << endl;
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
TramI::updateSchedule(const int interval, const Ice::Current &current) {
    cout << "\nSchedule updated." << endl;
    StopList schedule = tram_->getSchedule();
    for (StopInfo &info: schedule) {
        info.time.hour += interval / 60;
        info.time.minute += interval % 60;
    }
    //clear next trams i update
    tram_->setSchedule(schedule);
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
