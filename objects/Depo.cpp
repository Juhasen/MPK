#include <Ice/Ice.h>
#include "SIPI.h"

void
DepoI::TramOnline(const TramPrx &tram,
                  const Ice::Current &current) {
    auto now = chrono::system_clock::now();
    time_t now_time_t = chrono::system_clock::to_time_t(now);

    tm local_time{};
    localtime_r(&now_time_t, &local_time);

    TramInfo tramInfo;
    tramInfo.tram = tram;
    tramInfo.time.hour = local_time.tm_hour;
    tramInfo.time.minute = local_time.tm_min;

    online_trams_.push_back(tramInfo);
}

void
DepoI::TramOffline(const TramPrx &tram,
                   const Ice::Current &current) {
    auto it = std::remove_if(online_trams_.begin(), online_trams_.end(),
                             [&tram](const TramInfo &tramInfo) {
                                 return tramInfo.tram->getStockNumber() == tram->getStockNumber();
                             });
    if (it != online_trams_.end()) {
        online_trams_.erase(it, online_trams_.end());
    }
}

void
DepoI::showRegisteredTrams(const Ice::Current &current) {
    TramList trams = online_trams_;
    cout << "\n----------------" << endl;
    cout << "Registered trams:" << endl;
    cout << "----------------" << endl;
    for (const TramInfo &tramInfo: trams) {
        cout << "| " << tramInfo.tram->getStockNumber() << " |" << endl;
    }
    cout << "----------------" << endl;
}

string
DepoI::getName(const Ice::Current &current) {
    return name_;
}

void showMenu() {
    cout << "\nMENU:" << endl;
    cout << "1. Add Tram." << endl;
    cout << "2. Remove Tram." << endl;
    cout << "3. Show online trams." << endl;
    cout << "4. Set line for tram." << endl;
    cout << "0. Exit." << endl;
}

int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter depository configuration (depo_name/port): ";
    string input;
    cin >> input;
    string depo_name = input.substr(0, input.find('/'));
    string port = input.substr(input.find('/') + 1);
    cout << "Creating depo: " << depo_name << " on port " + port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("DepoAdapter", "default -p " + port);
        if (!adapter) {
            throw "Adapter creation failed";
        }
        Ice::ObjectPtr object = new DepoI(depo_name);
        adapter->add(object, Ice::stringToIdentity(depo_name));
        adapter->activate();
        cout << "Depo object created." << endl;

        DepoPrx depo = Ice::uncheckedCast<DepoPrx>(adapter->createProxy(Ice::stringToIdentity(depo_name)));

        while (true) {
            showMenu();
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
                    cout << "Enter tram configuration (tram_stock_number/port): ";
                    cin >> input;
                    string tram_stock_number = input.substr(0, input.find('/'));
                    string tram_port = input.substr(input.find('/') + 1);
                    cout << "Searching for tram with stock_number: " + tram_stock_number + " on port " + tram_port <<
                            endl;
                    Ice::ObjectPrx baseTram = ic->stringToProxy(tram_stock_number + ":default -p " + tram_port);
                    TramPrx tram = TramPrx::checkedCast(baseTram);
                    if (!tram)
                        throw "Invalid proxy";
                    depo->TramOnline(tram);
                    cout << "Tram registered." << endl;
                    break;
                }
                case '2': {
                    cout << "Enter tram configuration (tram_stock_number/port): ";
                    cin >> input;
                    string tram_stock_number = input.substr(0, input.find('/'));
                    string tram_port = input.substr(input.find('/') + 1);
                    cout << "Searching for tram with name: " + tram_stock_number + " on port " + tram_port << endl;
                    Ice::ObjectPrx baseTram = ic->stringToProxy(tram_stock_number + ":default -p " + tram_port);
                    TramPrx tram = TramPrx::checkedCast(baseTram);
                    if (!tram)
                        throw "Invalid proxy";
                    depo->TramOffline(tram);
                    cout << "Tram unregistered." << endl;
                    break;
                }
                case '3': {
                    depo->showRegisteredTrams();
                    break;
                }
                case '4': {
                    cout << "Enter mpk configuration (mpk_name/port): ";
                    cin >> input;
                    string mpk_name = input.substr(0, input.find('/'));
                    string mpk_port = input.substr(input.find('/') + 1);
                    cout << "Searching for mpk with name: " + mpk_name + " on port " + mpk_port << endl;
                    Ice::ObjectPrx baseMpk = ic->stringToProxy(mpk_name + ":default -p " + mpk_port);
                    MPKPrx mpk = MPKPrx::checkedCast(baseMpk);
                    if (!mpk)
                        throw "Invalid proxy";

                    LineList lines = mpk->getLines();
                    cout << "\n----------------" << endl;
                    cout << "Available lines:" << endl;
                    cout << "----------------" << endl;
                    for (LinePrx currLine: lines) {
                        cout << "| " << currLine->getName() << " |" << endl;
                    }
                    cout << "----------------" << endl;
                    cout << "Enter line name: ";
                    string line_name;
                    cin >> line_name;
                    LinePrx line = mpk->getLine(line_name);
                    if (!line)
                        throw "Invalid proxy";

                    cout << "Enter tram configuration (tram_stock_number/port): ";
                    cin >> input;
                    string tram_stock_number = input.substr(0, input.find('/'));
                    string tram_port = input.substr(input.find('/') + 1);
                    cout << "Searching for tram with name: " + tram_stock_number + " on port " + tram_port << endl;
                    Ice::ObjectPrx baseTram = ic->stringToProxy(tram_stock_number + ":default -p " + tram_port);
                    TramPrx tram = TramPrx::checkedCast(baseTram);
                    if (!tram)
                        throw "Invalid proxy";

                    tram->setLine(line);
                    cout << "Line set for tram." << endl;
                    cout << tram->getLine() << endl;
                    break;
                }
                default: {
                    cout << "Bad choice!" << endl;
                    break;
                }
            }
        }
        ic->shutdown();
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
