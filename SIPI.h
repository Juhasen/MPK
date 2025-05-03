#ifndef __SIPI_h__
#define __SIPI_h__

#include <SIP.h>

namespace SIP
{

class StopI : public virtual Stop
{
public:

    virtual ::std::string getName(const Ice::Current&) override;
};

class TramStopI : public virtual TramStop
{
public:

    virtual ::std::string getName(const Ice::Current&) override;

    virtual TramList getNextTrams(int,
                                  const Ice::Current&) override;

    virtual void RegisterPassenger(::std::shared_ptr<PassengerPrx>,
                                   const Ice::Current&) override;

    virtual void UnregisterPassenger(::std::shared_ptr<PassengerPrx>,
                                     const Ice::Current&) override;

    virtual void UpdateTramInfo(::std::shared_ptr<TramPrx>,
                                Time,
                                const Ice::Current&) override;
};

class LineI : public virtual Line
{
public:

    virtual TramList getTrams(const Ice::Current&) override;

    virtual StopList getStops(const Ice::Current&) override;

    virtual void registerTram(::std::shared_ptr<TramPrx>,
                              const Ice::Current&) override;

    virtual void unregisterTram(::std::shared_ptr<TramPrx>,
                                const Ice::Current&) override;

    virtual void setStops(StopList,
                          const Ice::Current&) override;

    virtual ::std::string getName(const Ice::Current&) override;
};

class LineFactoryI : public virtual LineFactory
{
public:

    virtual ::std::shared_ptr<LinePrx> createLine(::std::string,
                                                  const Ice::Current&) override;

    virtual double getLoad(const Ice::Current&) override;
};

class StopFactoryI : public virtual StopFactory
{
public:

    virtual ::std::shared_ptr<StopPrx> createStop(::std::string,
                                                  const Ice::Current&) override;

    virtual double getLoad(const Ice::Current&) override;
};

class MPKI : public virtual MPK
{
public:

    virtual ::std::shared_ptr<TramStopPrx> getTramStop(::std::string,
                                                       const Ice::Current&) override;

    virtual void registerDepo(::std::shared_ptr<DepoPrx>,
                              const Ice::Current&) override;

    virtual void unregisterDepo(::std::shared_ptr<DepoPrx>,
                                const Ice::Current&) override;

    virtual ::std::shared_ptr<DepoPrx> getDepo(::std::string,
                                               const Ice::Current&) override;

    virtual DepoList getDepos(const Ice::Current&) override;

    virtual LineList getLines(const Ice::Current&) override;

    virtual void addLine(::std::shared_ptr<LinePrx>,
                         const Ice::Current&) override;

    virtual void addStop(::std::shared_ptr<StopPrx>,
                         const Ice::Current&) override;

    virtual void registerLineFactory(::std::shared_ptr<LineFactoryPrx>,
                                     const Ice::Current&) override;

    virtual void unregisterLineFactory(::std::shared_ptr<LineFactoryPrx>,
                                       const Ice::Current&) override;

    virtual void registerStopFactory(::std::shared_ptr<StopFactoryPrx>,
                                     const Ice::Current&) override;

    virtual void unregisterStopFactory(::std::shared_ptr<StopFactoryPrx>,
                                       const Ice::Current&) override;
};

class DepoI : public virtual Depo
{
public:

    virtual void TramOnline(::std::shared_ptr<TramPrx>,
                            const Ice::Current&) override;

    virtual void TramOffline(::std::shared_ptr<TramPrx>,
                             const Ice::Current&) override;

    virtual ::std::string getName(const Ice::Current&) override;
};

class TramI : public virtual Tram
{
public:

    virtual ::std::shared_ptr<TramStopPrx> getLocation(const Ice::Current&) override;

    virtual ::std::shared_ptr<LinePrx> getLine(const Ice::Current&) override;

    virtual void setLine(::std::shared_ptr<LinePrx>,
                         const Ice::Current&) override;

    virtual StopList getNextStops(int,
                                  const Ice::Current&) override;

    virtual void RegisterPassenger(::std::shared_ptr<PassengerPrx>,
                                   const Ice::Current&) override;

    virtual void UnregisterPassenger(::std::shared_ptr<PassengerPrx>,
                                     const Ice::Current&) override;

    virtual ::std::string getStockNumber(const Ice::Current&) override;
};

class PassengerI : public virtual Passenger
{
public:

    virtual void updateTramInfo(::std::shared_ptr<TramPrx>,
                                StopList,
                                const Ice::Current&) override;

    virtual void updateStopInfo(::std::shared_ptr<StopPrx>,
                                TramList,
                                const Ice::Current&) override;
};

}

#endif
