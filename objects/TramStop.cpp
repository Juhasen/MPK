#include <Ice/Ice.h>
#include "SIPI.h"

string
TramStopI::getName(const Ice::Current &current) {
    return name_;
}

TramList
TramStopI::getNextTrams(int howMany,
                        const Ice::Current &current) {
    if (howMany <= 0) {
        return TramList();
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
    passengers_.push_back(passenger);
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

    StopList stop_list = tram->getNextStops(3);
    for (const auto &passenger: passengers_) {
        passenger->updateTramInfo(tram, stop_list);
    }
    cout << "Tram info updated." << endl;
}

int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter tram_stop configuration (tram_stop_name/host/port): ";
    string input;
    cin >> input;
    size_t first_slash = input.find('/');
    size_t second_slash = input.find('/', first_slash + 1);
    string tram_stop_name = input.substr(0, first_slash);
    string host = input.substr(first_slash + 1, second_slash - 1);
    string port = input.substr(second_slash + 1);
    cout << "Creating tram_stop: " << tram_stop_name << " on host " << host << " on port " + port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("TramStopAdapter", "tcp -h " + host + " -p " + port);
        Ice::ObjectPtr object = new TramStopI(tram_stop_name);
        adapter->add(object, Ice::stringToIdentity(tram_stop_name));
        adapter->activate();
        cout << "TramStop object created." << endl;

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
