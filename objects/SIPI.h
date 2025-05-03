#ifndef __SIPI_h__
#define __SIPI_h__

#include <memory>
#include <chrono>
#include <Ice/Ice.h>
#include "../SIP.h"

using namespace SIP;
using namespace std;

class StopI : public virtual Stop {
public:
    StopI(const string &name) : name_(name) {
    }

    virtual string getName(const Ice::Current &) override;

private:
    string name_;
};

class TramStopI : public virtual TramStop {
public:
    explicit TramStopI(const string &name) : name_(name) {
    }

    virtual string getName(const Ice::Current &) override;

    virtual TramList getNextTrams(int,
                                  const Ice::Current &) override;

    virtual void RegisterPassenger(const PassengerPrx &passenger_prx,
                                   const Ice::Current &) override;

    virtual void UnregisterPassenger(const PassengerPrx &passenger_prx,
                                     const Ice::Current &) override;

    virtual void UpdateTramInfo(const TramPrx &tram, const Time &time, const Ice::Current &) override;

private:
    string name_;
    TramList next_trams_;
    PassengerList passengers_;
};

class LineI : public virtual Line {
public:
    LineI(const std::string &name) : name_(name) {
    }

    virtual TramList getTrams(const Ice::Current &) override;

    virtual StopList getStops(const Ice::Current &) override;

    virtual void registerTram(const TramPrx &tram,
                              const Ice::Current &) override;

    virtual void unregisterTram(const TramPrx &tram,
                                const Ice::Current &) override;

    virtual void setStops(const StopList &stop_list,
                          const Ice::Current &) override;

    virtual string getName(const Ice::Current &) override;

private:
    string name_;
    StopList stops_;
    TramList trams_;
};

class LineFactoryI : public virtual LineFactory {
public:
    virtual LinePrx createLine(const string &name,
                               const Ice::Current &) override;

    virtual double getLoad(const Ice::Current &) override;
};

class StopFactoryI : public virtual StopFactory {
public:
    virtual StopPrx createStop(const string &name,
                               const Ice::Current &) override;

    virtual double getLoad(const Ice::Current &) override;
};

class MPKI : public virtual MPK {
public:
    virtual TramStopPrx getTramStop(const string &name,
                                    const Ice::Current &) override;

    virtual void registerDepo(const DepoPrx &depo,
                              const Ice::Current &) override;

    virtual void unregisterDepo(const DepoPrx &depo,
                                const Ice::Current &) override;

    virtual DepoPrx getDepo(const string &name,
                            const Ice::Current &) override;

    virtual DepoList getDepos(const Ice::Current &) override;

    virtual LineList getLines(const Ice::Current &) override;

    virtual LinePrx getLine(const string &name,
                              const Ice::Current &) override;

    virtual void registerLineFactory(const LineFactoryPrx &line_factory_prx,
                                     const Ice::Current &) override;

    virtual void unregisterLineFactory(const LineFactoryPrx &line_factory_prx,
                                       const Ice::Current &) override;

    virtual void registerStopFactory(const StopFactoryPrx &stop_factory_prx,
                                     const Ice::Current &) override;

    virtual void unregisterStopFactory(const StopFactoryPrx &stop_factory_prx,
                                       const Ice::Current &) override;

    virtual void addLine(const LinePrx &line,
                         const Ice::Current &) override;

    virtual void addStop(const StopPrx &stop,
                         const Ice::Current &) override;

    virtual void registerTramStop(const TramStopPrx &tram_stop_prx,
                                  const Ice::Current &) override;

private:
    LineList lines_;
    vector<TramStopPrx> tram_stops_;
    vector<StopPrx> stops_;
    DepoList depos_;
    LineFactoryPrx line_factory_;
    StopFactoryPrx stop_factory_;
};

class DepoI : public virtual Depo {
public:
    explicit DepoI(const string &name) : name_(name) {
    }

    virtual void TramOnline(const TramPrx &tram_prx,
                            const Ice::Current &) override;

    virtual void TramOffline(const TramPrx &tram_prx,
                             const Ice::Current &) override;

    virtual string getName(const Ice::Current &) override;

    virtual void showRegisteredTrams(const Ice::Current &) override;

private:
    string name_;
    TramList online_trams_;
};

class TramI : public virtual Tram {
public:
    explicit TramI(const string &stock_number) : stock_number_(stock_number) {
    }

    virtual TramStopPrx getLocation(const Ice::Current &) override;

    virtual LinePrx getLine(const Ice::Current &) override;

    virtual void setLine(const LinePrx &line_prx,
                         const Ice::Current &) override;

    virtual StopList getNextStops(int,
                                  const Ice::Current &) override;

    virtual void RegisterPassenger(const PassengerPrx &passenger_prx,
                                   const Ice::Current &) override;

    virtual void UnregisterPassenger(const PassengerPrx &passenger_prx,
                                     const Ice::Current &) override;

    virtual string getStockNumber(const Ice::Current &) override;

private:
    string stock_number_;
    LinePrx line_;
    TramStopPrx location_;
    PassengerList passengers_;
};

class PassengerI : public virtual Passenger {
public:
    virtual void updateTramInfo(const TramPrx &tram_prx,
                                const StopList &stop_list,
                                const Ice::Current &) override;

    virtual void updateStopInfo(const StopPrx &stop_prx,
                                const TramList &tram_list,
                                const Ice::Current &) override;
};


#endif
