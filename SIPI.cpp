
#include <SIPI.h>

::std::string
SIP::StopI::getName(const Ice::Current& current)
{
    return ::std::string();
}

::std::string
SIP::TramStopI::getName(const Ice::Current& current)
{
    return ::std::string();
}

::SIP::TramList
SIP::TramStopI::getNextTrams(int /*howMany*/,
                             const Ice::Current& current)
{
    return TramList();
}

void
SIP::TramStopI::RegisterPassenger(::std::shared_ptr<PassengerPrx> /*p*/,
                                  const Ice::Current& current)
{
}

void
SIP::TramStopI::UnregisterPassenger(::std::shared_ptr<PassengerPrx> /*p*/,
                                    const Ice::Current& current)
{
}

void
SIP::TramStopI::UpdateTramInfo(::std::shared_ptr<TramPrx> /*tram*/,
                               Time /*time*/,
                               const Ice::Current& current)
{
}

::SIP::TramList
SIP::LineI::getTrams(const Ice::Current& current)
{
    return TramList();
}

::SIP::StopList
SIP::LineI::getStops(const Ice::Current& current)
{
    return StopList();
}

void
SIP::LineI::registerTram(::std::shared_ptr<TramPrx> /*tram*/,
                         const Ice::Current& current)
{
}

void
SIP::LineI::unregisterTram(::std::shared_ptr<TramPrx> /*tram*/,
                           const Ice::Current& current)
{
}

void
SIP::LineI::setStops(StopList /*sl*/,
                     const Ice::Current& current)
{
}

::std::string
SIP::LineI::getName(const Ice::Current& current)
{
    return ::std::string();
}

::std::shared_ptr<::SIP::LinePrx>
SIP::LineFactoryI::createLine(::std::string /*name*/,
                              const Ice::Current& current)
{
    return nullptr;
}

double
SIP::LineFactoryI::getLoad(const Ice::Current& current)
{
    return 0.0;
}

::std::shared_ptr<::SIP::StopPrx>
SIP::StopFactoryI::createStop(::std::string /*name*/,
                              const Ice::Current& current)
{
    return nullptr;
}

double
SIP::StopFactoryI::getLoad(const Ice::Current& current)
{
    return 0.0;
}

::std::shared_ptr<::SIP::TramStopPrx>
SIP::MPKI::getTramStop(::std::string /*name*/,
                       const Ice::Current& current)
{
    return nullptr;
}

void
SIP::MPKI::registerDepo(::std::shared_ptr<DepoPrx> /*depo*/,
                        const Ice::Current& current)
{
}

void
SIP::MPKI::unregisterDepo(::std::shared_ptr<DepoPrx> /*depo*/,
                          const Ice::Current& current)
{
}

::std::shared_ptr<::SIP::DepoPrx>
SIP::MPKI::getDepo(::std::string /*name*/,
                   const Ice::Current& current)
{
    return nullptr;
}

::SIP::DepoList
SIP::MPKI::getDepos(const Ice::Current& current)
{
    return DepoList();
}

::SIP::LineList
SIP::MPKI::getLines(const Ice::Current& current)
{
    return LineList();
}

void
SIP::MPKI::addLine(::std::shared_ptr<LinePrx> /*l*/,
                   const Ice::Current& current)
{
}

void
SIP::MPKI::addStop(::std::shared_ptr<StopPrx> /*st*/,
                   const Ice::Current& current)
{
}

void
SIP::MPKI::registerLineFactory(::std::shared_ptr<LineFactoryPrx> /*lf*/,
                               const Ice::Current& current)
{
}

void
SIP::MPKI::unregisterLineFactory(::std::shared_ptr<LineFactoryPrx> /*lf*/,
                                 const Ice::Current& current)
{
}

void
SIP::MPKI::registerStopFactory(::std::shared_ptr<StopFactoryPrx> /*lf*/,
                               const Ice::Current& current)
{
}

void
SIP::MPKI::unregisterStopFactory(::std::shared_ptr<StopFactoryPrx> /*lf*/,
                                 const Ice::Current& current)
{
}

void
SIP::DepoI::TramOnline(::std::shared_ptr<TramPrx> /*t*/,
                       const Ice::Current& current)
{
}

void
SIP::DepoI::TramOffline(::std::shared_ptr<TramPrx> /*t*/,
                        const Ice::Current& current)
{
}

::std::string
SIP::DepoI::getName(const Ice::Current& current)
{
    return ::std::string();
}

::std::shared_ptr<::SIP::TramStopPrx>
SIP::TramI::getLocation(const Ice::Current& current)
{
    return nullptr;
}

::std::shared_ptr<::SIP::LinePrx>
SIP::TramI::getLine(const Ice::Current& current)
{
    return nullptr;
}

void
SIP::TramI::setLine(::std::shared_ptr<LinePrx> /*line*/,
                    const Ice::Current& current)
{
}

::SIP::StopList
SIP::TramI::getNextStops(int /*howMany*/,
                         const Ice::Current& current)
{
    return StopList();
}

void
SIP::TramI::RegisterPassenger(::std::shared_ptr<PassengerPrx> /*p*/,
                              const Ice::Current& current)
{
}

void
SIP::TramI::UnregisterPassenger(::std::shared_ptr<PassengerPrx> /*p*/,
                                const Ice::Current& current)
{
}

::std::string
SIP::TramI::getStockNumber(const Ice::Current& current)
{
    return ::std::string();
}

void
SIP::PassengerI::updateTramInfo(::std::shared_ptr<TramPrx> /*tram*/,
                                StopList /*stops*/,
                                const Ice::Current& current)
{
}

void
SIP::PassengerI::updateStopInfo(::std::shared_ptr<StopPrx> /*stop*/,
                                TramList /*trams*/,
                                const Ice::Current& current)
{
}
