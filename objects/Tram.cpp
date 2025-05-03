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