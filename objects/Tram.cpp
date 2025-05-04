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

LinePrx
TramI::getLine(const Ice::Current &current) {
    return line_;
}

void
TramI::setLine(const LinePrx &line,
               const Ice::Current &current) {
    line_ = line;
}

StopList
TramI::getNextStops(int howMany,
                    const Ice::Current &current) {
    if (howMany <= 0) {
        return StopList();
    }

    StopList stops = line_->getStops();
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
    cout << "Passenger registered." << endl;
}

void
TramI::UnregisterPassenger(const PassengerPrx &passenger,
                           const Ice::Current &current) {
    //find and remove passenger
    auto it = std::remove(passengers_.begin(), passengers_.end(), passenger);
    if (it != passengers_.end()) {
        passengers_.erase(it, passengers_.end());
    }
    cout << "Passenger unregistered." << endl;
}

void
TramI::updatePassengerInfo(const TramPrx& tram, const Ice::Current &current) {
    StopList stop_list = getNextStops(3, current);
    for (const auto &passenger: passengers_) {
        passenger->updateTramInfo(tram, stop_list);
    }
    cout << "Tram info updated." << endl;
}

string
TramI::getStockNumber(const Ice::Current &current) {
    return stock_number_;
}

int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter tram configuration (tram_stock_number/port): ";
    string input;
    cin >> input;
    string tram_stock_number = input.substr(0, input.find('/'));
    string port = input.substr(input.find('/') + 1);
    cout << "Creating tram: " << tram_stock_number << " on port " + port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("TramAdapter", "default -p " + port);
        Ice::ObjectPtr object = new TramI(tram_stock_number);
        adapter->add(object, Ice::stringToIdentity(tram_stock_number));
        adapter->activate();

        cout << "Tram object created." << endl;

        TramPrx tram = Ice::uncheckedCast<TramPrx>(adapter->createProxy(Ice::stringToIdentity(tram_stock_number)));
        if (!tram) {
            throw "Invalid proxy";
        }
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
                    cout << "Current position: " << tram->getLocation()->getName() << endl;
                    cout << "Assigned stops:" << endl;
                    StopList stops = tram->getLine()->getStops();
                    for (StopInfo stop: stops) {
                        cout << "Stop: " << stop.stop->getName() << endl;
                    }
                    break;
                }
                case 'n': {
                    cout << "Previous position: " << tram->getLocation()->getName() << endl;
                    tram->setLocation(tram->getNextStops(1)[0].stop);
                    cout << "New position: " << tram->getLocation()->getName() << endl;
                    tram->updatePassengerInfo(tram);
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
