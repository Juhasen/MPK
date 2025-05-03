#include <Ice/Ice.h>
#include "SIPI.h"

TramStopPrx
TramI::getLocation(const Ice::Current& current)
{
    return location_;
}

LinePrx
TramI::getLine(const Ice::Current& current)
{
    return line_;
}

void
TramI::setLine(const LinePrx& line,
                    const Ice::Current& current)
{
    line_ = line;
}

StopList
TramI::getNextStops(int howMany,
                         const Ice::Current& current)
{
    TramStopPrx stop = getLocation(current);
    if (howMany <= 0) {
        return StopList();
    }

    return line_->getStops();
}

void
TramI::RegisterPassenger(const PassengerPrx& passenger,
                              const Ice::Current& current)
{
    passengers_.push_back(passenger);
    cout<<"Passenger registered."<<endl;
}

void
TramI::UnregisterPassenger(const PassengerPrx& passenger,
                                const Ice::Current& current)
{
    //find and remove passenger
    auto it = std::remove(passengers_.begin(), passengers_.end(), passenger);
    if (it != passengers_.end()) {
        passengers_.erase(it, passengers_.end());
    }
    cout<<"Passenger unregistered."<<endl;
}

string
TramI::getStockNumber(const Ice::Current& current)
{
    return stock_number_;
}

int
main (int argc, char* argv[]) {
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("TramAdapter", "default -p 12347");
        cout << "Enter tram stock number: ";
        string stock_number;
        cin >> stock_number;
        Ice::ObjectPtr object = new TramI(stock_number);
        adapter->add(object, Ice::stringToIdentity("Tram"));
        adapter->activate();

        cout << "Tram object created." << endl;
        ic->waitForShutdown();
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception& e) {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}