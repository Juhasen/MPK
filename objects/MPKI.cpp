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

void MPKI::removeLine(const LinePrx &line, const Ice::Current &current) {
    string line_name = line->getName();
    auto it = std::remove(lines_.begin(), lines_.end(), line);
    if (it != lines_.end()) {
        lines_.erase(it, lines_.end());
    }
    cout << "Line removed: " << line_name << endl;
}

void MPKI::setStopsForLine(const LinePrx &line_prx, const string &filename, const Ice::Current &current) {
    std::ifstream file(filename);
    std::string txtLine;
    TramStopList stops;

    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku!" << std::endl;
        return;
    }

    while (std::getline(file, txtLine)) {
        std::istringstream iss(txtLine);
        std::string word, name, port;

        // Wczytaj port i nazwę przystanku
        iss >> port;
        while (iss >> word) {
            if (!name.empty()) { name += " "; }
            name += word;
        }

        if (port.empty() || name.empty()) {
            std::cerr << "Invalid line format: " << txtLine << std::endl;
            continue;
        }

        cout << "Creating tram_stop with name: " << name << " on port " << port << endl;

        try {
            Ice::ObjectAdapterPtr adapter = current.adapter;
            Ice::ObjectPtr tram_stop_object = new TramStopI(name);
            adapter->add(tram_stop_object, Ice::stringToIdentity(name));

            TramStopPrx tram_stop = Ice::uncheckedCast<TramStopPrx>(
                adapter->createProxy(Ice::stringToIdentity(name)));
            if (!tram_stop) {
                throw std::runtime_error("Invalid proxy");
            }

            registerTramStop(tram_stop, current);
            stops.push_back(tram_stop);
        } catch (const std::exception &e) {
            std::cerr << "Error creating tram_stop: " << e.what() << std::endl;
        }
    }

    file.close();
    line_prx->setStops(stops);
}

void MPKI::addStop(const StopPrx &stop_prx, const Ice::Current &current) {
    stops_.push_back(stop_prx);
}

void MPKI::registerTramStop(const TramStopPrx &tram_stop, const Ice::Current &current) {
    tram_stops_.push_back(tram_stop);
}
