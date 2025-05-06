#include <Ice/Ice.h>
#include "SIPI.h"

TramStopPrx
MPKI::getTramStop(const string &name,
                  const Ice::Current &current) {
    for (TramStopPrx stop: tram_stops_) {
        if (stop->getName() == name) {
            return stop;
        }
    }
    return nullptr;
}

TramStopList
MPKI::getTramStops(const Ice::Current &current) {
    return tram_stops_;
}

void
MPKI::registerDepo(const DepoPrx &depo,
                   const Ice::Current &current) {
    for (DepoInfo info: depos_) {
        if (info.name == depo->getName()) {
            cout << "Depo already registered." << endl;
            return;
        }
    }
    DepoInfo depo_info;
    depo_info.name = depo->getName();
    depo_info.stop = depo;
    depos_.push_back(depo_info);
    cout << "Depo registered: " << depo_info.name << endl;
}

void
MPKI::unregisterDepo(const DepoPrx &depo,
                     const Ice::Current &current) {
    auto it = std::remove_if(depos_.begin(), depos_.end(),
                             [&depo](const DepoInfo &depo_info) {
                                 return depo_info.stop->getName() == depo->getName();
                             });
    if (it != depos_.end()) {
        depos_.erase(it, depos_.end());
    }
    cout << "Depo unregistered." << endl;
}

DepoPrx
MPKI::getDepo(const string &name,
              const Ice::Current &current) {
    for (DepoInfo depo_info: depos_) {
        if (depo_info.name == name) {
            return depo_info.stop;
        }
    }
    return nullptr;
}

DepoList
MPKI::getDepos(const Ice::Current &current) {
    return depos_;
}

LineList
MPKI::getLines(const Ice::Current &current) {
    return lines_;
}

LinePrx
MPKI::getLine(const string &name,
              const Ice::Current &current) {
    for (LinePrx line: lines_) {
        if (line->getName() == name) {
            return line;
        }
    }
    return nullptr;
}

void
MPKI::registerLineFactory(const LineFactoryPrx &lf,
                          const Ice::Current &current) {
    line_factory_ = lf;
}

void
MPKI::unregisterLineFactory(const LineFactoryPrx &lf,
                            const Ice::Current &current) {
    line_factory_ = nullptr;
}

void
MPKI::registerStopFactory(const StopFactoryPrx &sf,
                          const Ice::Current &current) {
    stop_factory_ = sf;
}

void
MPKI::unregisterStopFactory(const StopFactoryPrx &sf,
                            const Ice::Current &current) {
    stop_factory_ = nullptr;
}

void MPKI::addLine(const LinePrx &line_prx, const Ice::Current &current) {
    lines_.push_back(line_prx);
}

void MPKI::setStopsForLine(const LinePrx &line_prx, const string &filename, const Ice::Current &current) {
    std::ifstream file(filename);
    std::string txtLine;
    StopList stops;

    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku!" << std::endl;
        return;
    }

    while (std::getline(file, txtLine)) {
        std::istringstream iss(txtLine);
        StopInfo stop;
        std::string time, word, name = "";

        // Wczytaj godzinę
        iss >> time;
        stop.time.hour = stoi(time.substr(0, time.find_last_of(':')));
        stop.time.minute = stoi(time.substr(time.find_last_of(':') + 1));

        string host;
        iss >> host;

        string port;
        iss >> port;

        // Reszta to nazwa przystanku (może mieć spacje)
        while (iss >> word) {
            if (!name.empty()) { name += " "; }
            name += word;
        }

        //mamy name, znajdź TramStopPrx z name i przypisz do stop
        string tram_stop_name = name;
        string tram_stop_port = port;
        string tram_stop_host = host;
        cout << "Searching for tram_stop with name: " + tram_stop_name + " on host " + host + " port " + tram_stop_port
                << endl;
        Ice::CommunicatorPtr ic = current.adapter->getCommunicator();
        Ice::ObjectPrx baseTramStop = ic->stringToProxy(tram_stop_name + ":tcp -h " + host + " -p " + tram_stop_port);
        TramStopPrx tram_stop = TramStopPrx::checkedCast(baseTramStop);
        if (!tram_stop) {
            throw "Invalid proxy";
        }
        // TODO: dodawać same tramstopy
        stop.stop = tram_stop;
        registerTramStop(tram_stop, current);
        stops.push_back(stop);
    }
//TODO: w depo mapa <tramstop, TRAMLIST<czas, tramwaj>>, dla tramstopa ustalić czas + tramwaj
    file.close();
    line_prx->setStops(stops);
}

void MPKI::addStop(const StopPrx &stop_prx, const Ice::Current &current) {
    stops_.push_back(stop_prx);
}

void MPKI::registerTramStop(const TramStopPrx &tram_stop, const Ice::Current &current) {
    tram_stops_.push_back(tram_stop);
}


void showMenu() {
    cout << "\nMENU:" << endl;
    cout << "1. Create line." << endl;
    cout << "2. Show lines." << endl;
    cout << "3. Register depo." << endl;
    cout << "4. Unregister depo." << endl;
    cout << "5. Show depos." << endl;
    cout << "6. Load stops from file." << endl;
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
    cout << "Creating mpk: " << mpk_name << " on host " << host << " on port " + port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("MpkAdapter", "tcp -h " + host + " -p " + port);
        Ice::ObjectPtr object = new MPKI;
        adapter->add(object, Ice::stringToIdentity(mpk_name));
        adapter->activate();

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

        MPKPrx mpk = Ice::uncheckedCast<MPKPrx>(adapter->createProxy(Ice::stringToIdentity(mpk_name)));
        mpk->registerLineFactory(lineFactory);

        while (true) {
            showMenu();
            cout << "Enter your choice: ";
            char choice = '0';
            cin >> choice;
            if (choice == '0') {
                cout << "Exiting..." << endl;
                break;
            }
            switch (choice) {
                case '1': {
                    string line_name;
                    cout << "Enter line name: ";
                    cin >> line_name;
                    LinePrx line = lineFactory->createLine(line_name);
                    mpk->addLine(line);

                    break;
                }
                case '2': {
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
                case '3': {
                    cout << "Enter depository configuration (depo_name/host/port): ";
                    string input;
                    cin >> input;

                    size_t first_slash = input.find('/');
                    size_t second_slash = input.find('/', first_slash + 1);

                    string depo_name = input.substr(0, first_slash);
                    string host = input.substr(first_slash + 1, second_slash - first_slash - 1);
                    string depo_port = input.substr(second_slash + 1);

                    cout << "Searching for depo with name: " << depo_name << " on host " << host << " on port " <<
                            depo_port << endl;

                    Ice::ObjectPrx baseDepo = ic->stringToProxy(depo_name + ":tcp -h " + host + " -p " + depo_port);
                    DepoPrx depo = DepoPrx::checkedCast(baseDepo);
                    if (!depo) {
                        throw "Invalid proxy";
                    }
                    mpk->registerDepo(depo);
                    break;
                }
                case '4': {
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
                case '5': {
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
                case '6': {
                    //List lines and find line with the same name and give its proxy
                    LineList lines = mpk->getLines();
                    if (lines.empty()) {
                        cout << "No registered lines available." << endl;
                        break;
                    }
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
                    if (!line) {
                        throw "Invalid proxy";
                    }

                    cout << "Enter file name: ";
                    string file_name;
                    cin >> file_name;
                    mpk->setStopsForLine(line, file_name);
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
