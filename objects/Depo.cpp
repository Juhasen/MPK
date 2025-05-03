#include <Ice/Ice.h>
#include <SIPI.h>

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

