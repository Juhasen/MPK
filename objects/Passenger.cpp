#include <Ice/Ice.h>
#include "SIPI.h"

void
PassengerI::updateTramInfo(const TramPrx &tram,
                           const StopList &stops,
                           const Ice::Current &current) {
    cout << "\nTram updated: " << tram->getStockNumber() << endl;
    for (const StopInfo &stop: stops) {
        cout << "Stop: " << stop.stop->getName() << endl;
    }
}

void
PassengerI::updateStopInfo(const StopPrx &stop,
                           const TramList &trams,
                           const Ice::Current &current) {
    // Update the stop information
    cout << "\nStop updated: " << stop->getName() << endl;
    for (const TramInfo &tram: trams) {
        cout << "Tram: " << tram.tram->getStockNumber() << endl;
    }
}

int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter passenger configuration (passenger_name/port): ";
    string input;
    cin >> input;
    string passenger_name = input.substr(0, input.find('/'));
    string port = input.substr(input.find('/') + 1);
    cout << "Creating passenger: " << passenger_name << " on port " + port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("PassengerAdapter", "default -p " + port);
        Ice::ObjectPtr object = new PassengerI;
        adapter->add(object, Ice::stringToIdentity(passenger_name));
        adapter->activate();

        PassengerPrx passenger = PassengerPrx::uncheckedCast(adapter->createProxy(
            Ice::stringToIdentity(passenger_name)));
        cout << "Passenger object created." << endl;

        cout << "Enter mpk configuration (mpk_name/port): ";
        cin >> input;
        string mpk_name = input.substr(0, input.find('/'));
        string mpk_port = input.substr(input.find('/') + 1);
        cout << "Searching for mpk with name: " + mpk_name + " on port " + mpk_port << endl;
        Ice::ObjectPrx baseMpk = ic->stringToProxy(mpk_name + ":default -p " + mpk_port);
        MPKPrx mpk = MPKPrx::checkedCast(baseMpk);
        if (!mpk)
            throw "Invalid proxy";

        cout << "Passenger connected to MPK." << endl;

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
                                tram->RegisterPassenger(passenger);
                                cout << "Passenger registered." << endl;
                                break;
                            }
                        }
                    } else if (object_type == "s") {
                        TramStopList tram_stops  = mpk->getTramStops();
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
                        tram_stop->RegisterPassenger(passenger);

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
