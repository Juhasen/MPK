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
