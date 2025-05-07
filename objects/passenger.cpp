#include <iomanip>
#include <Ice/Ice.h>
#include "SIPI.h"

void
PassengerI::updateTramInfo(const TramPrx &tram,
                           const StopList &stops,
                           const Ice::Current &current) {
    cout << "\nTram updated: " << tram->getStockNumber() << endl;
    for (const StopInfo& info : stops) {
        // ustawiamy szerokość na 2 znaki i wypełniacz '0'
        std::cout
            << "Stop: " << info.stop->getName() << " Time: "
            << std::setw(2) << std::setfill('0') << info.time.hour
            << ":"
            << std::setw(2) << std::setfill('0') << info.time.minute
            << std::endl;
    }
}

void
PassengerI::updateTramStopInfo(const TramPrx &tram,
                           const Ice::Current &current) {
    cout << "\nTram " << tram->getStockNumber() << " reached registered tram stop" << endl;
}

void
PassengerI::setTram(const TramPrx &tram, const Ice::Current &current) {
    tram_ = tram;
}

TramPrx
PassengerI::getTram(const Ice::Current &current) {
    return tram_;
}

void
PassengerI::setTramStop(const TramStopPrx &stop, const Ice::Current &current) {
    stop_ = stop;
}

TramStopPrx
PassengerI::getTramStop(const Ice::Current &current) {
    return stop_;
}

int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter passenger configuration (passenger_name/port): ";
    string input;
    cin >> input;
    size_t first_slash = input.find('/');
    string passenger_name = input.substr(0, first_slash);
    string port = input.substr(first_slash + 1);
    string host = getNetworkInterface();
    if (!isPortBusy(host, port)) {
        cerr << "Port " << port << " is not available." << endl;
        return 1;
    }
    cout << "Creating passenger: " << passenger_name << " on host " << host << " on port " + port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("PassengerAdapter", "tcp -h " + host + " -p " + port);
        Ice::ObjectPtr object = new PassengerI;
        adapter->add(object, Ice::stringToIdentity(passenger_name));
        adapter->activate();
        auto comm = ic;
        std::thread([comm]() {
            comm->waitForShutdown();
        }).detach();

        PassengerPrx passenger = PassengerPrx::uncheckedCast(
            adapter->createProxy(Ice::stringToIdentity(passenger_name))
        );
        cout << "Passenger object created." << endl;

        cout << "Enter mpk configuration (mpk_name/host/port): ";
        cin >> input;
        size_t mpk_first_slash = input.find('/');
        size_t mpk_second_slash = input.find('/', mpk_first_slash + 1);
        string mpk_name = input.substr(0, mpk_first_slash);
        string mpk_host = input.substr(mpk_first_slash + 1, mpk_second_slash - mpk_first_slash - 1);
        string mpk_port = input.substr(mpk_second_slash + 1);

        cout << "Searching for mpk with name: " << mpk_name
                << " on host " << mpk_host << " on port " << mpk_port << endl;

        Ice::ObjectPrx baseMpk = ic->stringToProxy(mpk_name + ":tcp -h " + mpk_host + " -p " + mpk_port);
        MPKPrx mpk = MPKPrx::checkedCast(baseMpk);
        if (!mpk)
            throw "Invalid proxy";

        std::thread(monitorMPKConnection, mpk, ic).detach();
        cout << "Passenger connected to MPK." << endl;

        passenger->setTram(nullptr);
        passenger->setTramStop(nullptr);

        while (true) {
            cout << "\nMENU:" << endl;
            cout << "1. Register." << endl;
            cout << "2. Unregister." << endl;
            cout << "0. Exit." << endl;
            cout << "Enter your choice: ";
            char choice = '0';
            cin >> choice;
            if (choice == '0') {
                cout << "Exiting..." << endl;
                break;
            }
            string input;
            switch (choice) {
                case '1': {
                    string object_type;
                    cout << "Enter type of object tram/stop (t/s): " << endl;
                    cin >> object_type;

                    if (object_type == "t") {
                        DepoList depos = mpk->getDepos();
                        cout << "Available trams:" << endl;
                        for (DepoInfo depo: depos) {
                            TramList trams = depo.stop->getOnlineTrams();
                            for (const TramInfo &tramInfo: trams) {
                                cout << "| " << tramInfo.tram->getStockNumber() << " |" << endl;
                            }
                        }
                        cout << "Enter tram stock number: ";
                        string tram_stock_number;
                        cin >> tram_stock_number;
                        for (DepoInfo depo: depos) {
                            TramPrx tram = depo.stop->getTram(tram_stock_number);
                            if (tram) {
                                cout << "Tram found: " << tram->getStockNumber() << endl;
                                cout << "Registering passenger..." << endl;
                                passenger->setTram(tram);
                                tram->RegisterPassenger(passenger);
                                cout << "Passenger registered." << endl;
                                if (passenger->getTramStop()) {
                                    passenger->getTramStop()->UnregisterPassenger(passenger);
                                    passenger->setTramStop(nullptr);
                                    cout << "Passenger unregistered from TramStop." << endl;
                                    break;
                                }
                                break;
                            }
                        }
                    } else if (object_type == "s") {
                        TramStopList tram_stops = mpk->getTramStops();
                        cout << "Available stops:" << endl;
                        for (TramStopPrx stop: tram_stops) {
                            cout << "| " << stop->getName() << " |" << endl;
                        }
                        cout << "Enter stop name: ";
                        string stop_name;
                        cin >> stop_name;
                        TramStopPrx tram_stop = mpk->getTramStop(stop_name);
                        if (!tram_stop)
                            throw "Invalid proxy";
                        passenger->setTramStop(tram_stop);
                        tram_stop->RegisterPassenger(passenger);
                        cout << "\nPassenger registered to TramStop." << endl;
                        if (passenger->getTram()) {
                            passenger->getTram()->UnregisterPassenger(passenger);
                            passenger->setTram(nullptr);
                            cout << "Passenger unregistered from Tram." << endl;
                        }
                    } else {
                        cout << "Invalid type!" << endl;
                    }
                    break;
                }
                case '2': {
                    string object_type;
                    cout << "Enter type of object tram/stop (t/s): " << endl;
                    cin >> object_type;

                    if (object_type == "t") {
                        DepoList depos = mpk->getDepos();
                        cout << "Available trams:" << endl;
                        for (DepoInfo depo: depos) {
                            TramList trams = depo.stop->getOnlineTrams();
                            for (const TramInfo &tramInfo: trams) {
                                cout << "| " << tramInfo.tram->getStockNumber() << " |" << endl;
                            }
                        }
                        cout << "Enter tram stock number: ";
                        string tram_stock_number;
                        cin >> tram_stock_number;
                        for (DepoInfo depo: depos) {
                            TramPrx tram = depo.stop->getTram(tram_stock_number);
                            if (tram) {
                                cout << "Tram found: " << tram->getStockNumber() << endl;
                                cout << "Unregistering passenger..." << endl;
                                tram->UnregisterPassenger(passenger);
                                cout << "Passenger unregistered." << endl;
                                break;
                            }
                        }
                    } else if (object_type == "s") {
                        TramStopList tram_stops = mpk->getTramStops();
                        cout << "Available stops:" << endl;
                        for (TramStopPrx stop: tram_stops) {
                            cout << "| " << stop->getName() << " |" << endl;
                        }
                        cout << "Enter stop name: ";
                        string stop_name;
                        cin >> stop_name;
                        TramStopPrx tram_stop = mpk->getTramStop(stop_name);
                        if (!tram_stop)
                            throw "Invalid proxy";
                        tram_stop->UnregisterPassenger(passenger);
                        cout << "\nPassenger unregistered from TramStop." << endl;
                    } else {
                        cout << "Invalid type!" << endl;
                    }
                    break;
                }
                default: {
                    cout << "Bad choice!" << endl;
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
            ic->destroy();
        } catch (const Ice::Exception &e) {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}
