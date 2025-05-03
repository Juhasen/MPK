#include <Ice/Ice.h>
#include "SIPI.h"

void
PassengerI::updateTramInfo(const TramPrx& tram,
                                const StopList& stops,
                                const Ice::Current& current)
{
    // Update the tram information
    cout << "Tram updated: " << tram->getStockNumber() << endl;
    for (const StopInfo& stop : stops) {
        cout << "Stop: " << stop.stop->getName() << endl;
    }
}

void
PassengerI::updateStopInfo(const StopPrx& stop,
                                const TramList& trams,
                                const Ice::Current& current)
{
    // Update the stop information
    cout << "Stop updated: " << stop->getName() << endl;
    for (const TramInfo& tram : trams) {
        cout << "Tram: " << tram.tram->getStockNumber() << endl;
    }
}

int
main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("PassengerAdapter", "default -p 12348");
        Ice::ObjectPtr object = new PassengerI;
        adapter->addWithUUID(object);
        adapter->activate();
        cout << "Passenger object created." << endl;
        cout << "With UUID: " << object->ice_id() << endl;
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