#include <iomanip>
#include <mutex>

#include "SIPI.h"

std::mutex tram_mutex;
std::vector<TramPrx> registered_trams_monitoring;

void monitorTrams(MPKPrx mpk) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::lock_guard<std::mutex> lock(tram_mutex);
        for (auto it = registered_trams_monitoring.begin(); it != registered_trams_monitoring.end();) {
            try {
                // Ustawienie limitu czasu na 2 sekundy
                TramPrx tram = (*it)->ice_timeout(2000);
                tram->ice_ping(); // Sprawdzenie dostępności
                ++it;
            } catch (const Ice::Exception &e) {
                std::cerr << "\nTram unresponsive. Unregistering..." << std::endl;
                try {
                    LinePrx line = (*it)->getLine();
                    line->unregisterTram(*it);
                } catch (...) {
                    std::cerr << "Failed to unregister tram from line." << std::endl;
                }
                try {
                    DepoPrx depo = (*it)->getDepo();
                    depo->TramOffline(*it);
                } catch (...) {
                    std::cerr << "Failed to unregister tram from depo." << std::endl;
                }
                it = registered_trams_monitoring.erase(it);
            }
        }
    }
}

namespace std {
    class mutex;
}

void showMenu() {
    cout << "\nMENU:" << endl;
    cout << "1. Create line." << endl;
    cout << "2. Remove line." << endl;
    cout << "3. Show lines." << endl;
    cout << "4. Register depo." << endl;
    cout << "5. Unregister depo." << endl;
    cout << "6. Show depos." << endl;
    cout << "7. Register tram." << endl;
    cout << "8. Unregister tram." << endl;
    cout << "9. Show online trams." << endl;
    cout << "10. Get line factory load." << endl;
    cout << "0. Exit." << endl;
}

int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter mpk configuration (mpk_name/port): ";
    string input;
    cin >> input;
    size_t first_slash = input.find('/');
    string mpk_name = input.substr(0, first_slash);
    string port = input.substr(first_slash + 1);
    string host = getNetworkInterface();
    if (!isPortBusy(host, port)) {
        cerr << "Port " << port << " is not available." << endl;
        return 1;
    }
    cout << "Creating mpk: " << mpk_name << " on host " << host << " on port " + port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("MpkAdapter", "tcp -h " + host + " -p " + port);
        Ice::ObjectPtr object = new MPKI;
        adapter->add(object, Ice::stringToIdentity(mpk_name));
        adapter->activate();
        MPKPrx mpk = Ice::uncheckedCast<MPKPrx>(adapter->createProxy(Ice::stringToIdentity(mpk_name)));
        cout << "Creating MPK server..." << endl;

        //Register line

        // Register line factory
        cout << "Enter line factory configuration (name/host/port): ";
        cin >> input;

        first_slash = input.find('/');
        int second_slash = input.find('/', first_slash + 1);

        string line_factory_name = input.substr(0, first_slash);
        string line_host = input.substr(first_slash + 1, second_slash - first_slash - 1);
        string line_port = input.substr(second_slash + 1);

        cout << "Connecting to LineFactory: " << line_factory_name << " on host " << line_host << " port " << line_port
                << endl;

        Ice::ObjectPrx baseLine = ic->stringToProxy(line_factory_name + ":tcp -h " + line_host + " -p " + line_port);

        LineFactoryPrx lineFactory = LineFactoryPrx::checkedCast(baseLine);
        if (!lineFactory) {
            throw "Invalid proxy for LineFactory";
        }

        mpk->registerLineFactory(lineFactory);

        std::thread monitor_thread(monitorTrams, mpk);
        monitor_thread.detach();

        //Main loop
        while (true) {
            showMenu();
            cout << "Enter your choice: ";
            int choice = 0;
            cin >> choice;
            if (choice == 0) {
                cout << "Exiting..." << endl;
                break;
            }
            switch (choice) {
                case 1: {
                    string line_name;
                    cout << "Enter line name: ";
                    cin >> line_name;
                    LinePrx line = lineFactory->createLine(line_name);
                    mpk->addLine(line);
                    cout << "Enter file name (with stop list): ";
                    string file_name;
                    cin >> file_name;
                    mpk->setStopsForLine(line, file_name);
                    break;
                }
                case 2: {
                    LineList lines = mpk->getLines();
                    if (lines.empty()) {
                        cout << "No registered lines available." << endl;
                        break;
                    }
                    cout << "\n----------------" << endl;
                    cout << "Available lines:" << endl;
                    cout << "----------------" << endl;
                    for (LinePrx currLine: lines) {
                        cout << "| " << currLine.get()->getName() << " |" << endl;
                    }
                    cout << "----------------" << endl;
                    cout << "Enter line name: ";
                    string line_name;
                    cin >> line_name;
                    LinePrx line = mpk->getLine(line_name);
                    if (!line) {
                        cout << "Line with name '" << line_name << "' not found." << endl;
                        break;
                    }
                    mpk->removeLine(line);
                    break;
                }
                case 3: {
                    LineList lines = mpk->getLines();
                    cout << "\n----------------" << endl;
                    cout << "Available lines:" << endl;
                    cout << "----------------" << endl;
                    for (LinePrx currLine: lines) {
                        cout << "| " << currLine.get()->getName() << " |" << endl;
                    }
                    cout << "----------------" << endl;
                    break;
                }
                case 4: {
                    cout << "Enter depository configuration (depo_name/port): ";
                    string input;
                    cin >> input;
                    size_t slash = input.find('/');
                    string depo_name = input.substr(0, slash);
                    string depo_port = input.substr(slash + 1);

                    cout << "Creating depo with name: " << depo_name << " on host " << host << " on port " << depo_port
                            << endl;

                    Ice::ObjectPtr depo_object = new DepoI(depo_name);
                    adapter->add(depo_object, Ice::stringToIdentity(depo_name));

                    DepoPrx depo = Ice::uncheckedCast<DepoPrx>(
                        adapter->createProxy(Ice::stringToIdentity(depo_name)));

                    mpk->registerDepo(depo);
                    break;
                }
                case 5: {
                    DepoList depos = mpk->getDepos();
                    if (depos.empty()) {
                        cout << "No registered depositories available." << endl;
                        break;
                    }

                    cout << "\n----------------" << endl;
                    cout << "Available depositories:" << endl;
                    cout << "----------------" << endl;
                    for (const DepoInfo &currDepo: depos) {
                        cout << "| " << currDepo.name << " |" << endl;
                    }
                    cout << "----------------" << endl;

                    cout << "Enter the name of the depository to unregister: ";
                    string depo_name;
                    cin >> depo_name;

                    DepoPrx depo = mpk->getDepo(depo_name);
                    if (!depo) {
                        cout << "Depo with name '" << depo_name << "' not found." << endl;
                        break;
                    }

                    mpk->unregisterDepo(depo);
                    cout << "Depo unregistered: " << depo_name << endl;
                    break;
                }
                case 6: {
                    DepoList depos = mpk->getDepos();
                    cout << "\n----------------" << endl;
                    cout << "Available depositories:" << endl;
                    cout << "----------------" << endl;
                    for (DepoInfo currDepo: depos) {
                        cout << "| " << currDepo.name << " |" << endl;
                    }
                    cout << "----------------" << endl;
                    break;
                }
                case 7: {
                    cout << "Enter tram configuration (tram_stock_number/host/port): ";
                    cin >> input;

                    first_slash = input.find('/');
                    int second_slash = input.find('/', first_slash + 1);
                    string tram_stock_number = input.substr(0, first_slash);
                    host = input.substr(first_slash + 1, second_slash - first_slash - 1);
                    string tram_port = input.substr(second_slash + 1);

                    cout << "Searching for tram with stock_number: " << tram_stock_number
                            << " on host " << host << " on port " << tram_port << endl;

                    Ice::ObjectPrx baseTram = ic->stringToProxy(
                        tram_stock_number + ":tcp -h " + host + " -p " + tram_port);
                    TramPrx tram = TramPrx::checkedCast(baseTram);
                    if (!tram)
                        throw "Invalid proxy";

                    //show depos
                    DepoList depos = mpk->getDepos();
                    if (depos.empty()) {
                        cout << "No registered depositories available." << endl;
                        break;
                    }
                    cout << "\n----------------" << endl;
                    cout << "Available depositories:" << endl;
                    cout << "----------------" << endl;
                    for (DepoInfo currDepo: depos) {
                        cout << "| " << currDepo.name << " |" << endl;
                    }
                    cout << "----------------" << endl;

                    string depo_name;
                    cout << "Enter the name of the depository to register the tram: ";
                    cin >> depo_name;
                    DepoPrx depo = mpk->getDepo(depo_name);
                    depo->TramOnline(tram);

                    cout << "Tram assigned to depo." << endl;

                    LineList lines = mpk->getLines();
                    if (lines.empty()) {
                        cout << "No registered lines available." << endl;
                        break;
                    }
                    cout << "\n----------------" << endl;
                    cout << "Available lines:" << endl;
                    cout << "----------------" << endl;
                    for (LinePrx currLine: lines) {
                        cout << "| " << currLine.get()->getName() << " |" << endl;
                    }
                    cout << "----------------" << endl;
                    string line_name;
                    cout << "Enter line name: ";
                    cin >> line_name;
                    LinePrx line = mpk->getLine(line_name);
                    if (!line) {
                        cout << "Line with name '" << line_name << "' not found." << endl;
                        break;
                    }
                    line->registerTram(tram);
                    tram->setLine(line);
                    tram->setLocation(line->getStops()[0]);
                    tram->setDepo(depo);
                    cout << "Tram assigned to line." << endl;

                    cout << "Enter interval for tram stop (in minutes): ";
                    int interval;
                    cin >> interval;

                    // 1. Pobranie aktualnego czasu systemowego
                    auto now = std::chrono::system_clock::now();
                    std::time_t t = std::chrono::system_clock::to_time_t(now);
                    tm localTime = *std::localtime(&t);
                    int hour = localTime.tm_hour;
                    int minute = localTime.tm_min;

                    TramStopList tramStops = line->getStops();
                    //schedule
                    StopList schedule;
                    const int minutesInDay = 24 * 60;
                    int startTotal = hour * 60 + minute;

                    schedule.clear();
                    schedule.reserve(tramStops.size());

                    for (size_t i = 0; i < tramStops.size(); ++i) {
                        int stopTotal = (startTotal + interval * static_cast<int>(i)) % minutesInDay;
                        if (stopTotal < 0) stopTotal += minutesInDay;
                        // zabezpieczenie, gdy i lub interval mogłyby być ujemne

                        StopInfo info;
                        info.stop = tramStops[i];
                        info.time.hour = stopTotal / 60;
                        info.time.minute = stopTotal % 60;
                        schedule.push_back(info);
                    }

                    tram->setSchedule(schedule);
                    //show schedule
                    cout << "Tram schedule:" << endl;
                    for (const StopInfo &info: schedule) {
                        // ustawiamy szerokość na 2 znaki i wypełniacz '0'
                        std::cout
                                << "Stop: " << info.stop->getName() << " Time: "
                                << std::setw(2) << std::setfill('0') << info.time.hour
                                << ":"
                                << std::setw(2) << std::setfill('0') << info.time.minute
                                << std::endl;
                    }


                    lock_guard<std::mutex> lock(tram_mutex);
                    registered_trams_monitoring.push_back(tram);
                    cout << "Tram successfully registered." << endl;
                    break;
                }
                case 8: {
                    cout << "----------------" << endl;
                    cout << "Online trams:" << endl;
                    cout << "----------------" << endl;
                    DepoList depos = mpk->getDepos();
                    for (DepoInfo depo: depos) {
                        TramList trams = depo.stop->getOnlineTrams();
                        cout << "Depo: " << depo.name << endl;
                        for (const TramInfo &tramInfo: trams) {
                            cout << "| " << tramInfo.tram->getStockNumber() << " |" << endl;
                        }
                    }
                    cout << "----------------" << endl;
                    string tram_stock_number;
                    cout << "Enter tram stock number: ";
                    cin >> tram_stock_number;
                    DepoPrx depo = nullptr;
                    TramPrx tram = nullptr;
                    for (DepoInfo depoInfo: depos) {
                        TramPrx tram = depoInfo.stop->getTram(tram_stock_number);
                        if (tram) {
                            depo = depoInfo.stop;
                            break;
                        }
                    }
                    if (!depo) {
                        cout << "Tram with stock number '" << tram_stock_number << "' not found." << endl;
                        break;
                    }
                    LinePrx line = tram->getLine();
                    line->unregisterTram(tram);
                    depo->TramOffline(tram);
                    std::lock_guard<std::mutex> lock(tram_mutex);
                    registered_trams_monitoring.erase(
                        std::remove(registered_trams_monitoring.begin(), registered_trams_monitoring.end(), tram),
                        registered_trams_monitoring.end());
                    cout << "Tram unregistered." << endl;
                    break;
                }
                case 9: {
                    cout << "----------------" << endl;
                    cout << "Online trams:" << endl;
                    cout << "----------------" << endl;
                    DepoList depos = mpk->getDepos();
                    for (DepoInfo depo: depos) {
                        TramList trams = depo.stop->getOnlineTrams();
                        cout << "Depo: " << depo.name << endl;

                        for (const TramInfo &tramInfo: trams) {
                            try {
                                cout << "| " << tramInfo.tram->getStockNumber() << " |" << endl;
                            } catch (Ice::Exception &e) {
                                cout << "Probably lost connection with this tram |" << endl;
                            }
                        }
                    }
                    cout << "----------------" << endl;
                    break;
                }
                case 10: {
                    cout << "Line factory load: " << lineFactory->getLoad() << endl;
                    break;
                }
                default: {
                    cout << "Bad choice!" << endl;
                    break;
                }
            }
        }
    } catch (const Ice::Exception &ex) {
        cerr << "Exception: " << ex << endl;
        status = 1;
    } catch (const char *msg) {
        cerr << "Msg:" << msg << endl;
        status = 1;
    }
    if (ic) {
        ic->shutdown();
    }
    ic->destroy();
    return status;
}
