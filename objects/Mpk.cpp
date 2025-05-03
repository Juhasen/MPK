#include <Ice/Ice.h>
#include "SIPI.h"

TramStopPrx
MPKI::getTramStop(const string& name,
                       const Ice::Current& current)
{
    return nullptr;
}

void
MPKI::registerDepo(const DepoPrx& depo,
                        const Ice::Current& current)
{
}

void
MPKI::unregisterDepo(const DepoPrx& depo,
                          const Ice::Current& current)
{
}

DepoPrx
MPKI::getDepo(const string& name,
                   const Ice::Current& current)
{
    return nullptr;
}

DepoList
MPKI::getDepos(const Ice::Current& current)
{
    return DepoList();
}

LineList
MPKI::getLines(const Ice::Current& current)
{
    return LineList();
}

void
MPKI::registerLineFactory(const LineFactoryPrx& lf,
                               const Ice::Current& current)
{
    Ice::CommunicatorPtr comm = current.adapter->getCommunicator();
    Ice::ObjectPrx baseLine = comm->stringToProxy("LineFactory:default -p 10000");
    LineFactoryPrx lineFactory = LineFactoryPrx::checkedCast(baseLine);
    if (!lineFactory)
        throw "Invalid proxy";
}

void
MPKI::unregisterLineFactory(const LineFactoryPrx& lf,
                                 const Ice::Current& current)
{
}

void
MPKI::registerStopFactory(const StopFactoryPrx& sf,
                               const Ice::Current& current)
{
}

void
MPKI::unregisterStopFactory(const StopFactoryPrx& sf,
                                 const Ice::Current& current)
{
}


int
main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("MpkAdapter", "default -p 12345");
        Ice::ObjectPtr object = new MPKI;
        adapter->add(object, Ice::stringToIdentity("Mpk"));
        adapter->activate();

        MPKPrx mpk = Ice::uncheckedCast<MPKPrx>(adapter->createProxy(Ice::stringToIdentity("Mpk")));
        LineFactoryPrx lineFactory = nullptr;
        mpk->registerLineFactory(lineFactory);


//        LineFactoryPrx lineFactory = nullptr;
//        MPKI::registerLineFactory(linefactory, ic);
        /*
        Ice::ObjectPrx baseLine = ic->stringToProxy("LineFactory:default -p 10000");
        LineFactoryPrx lineFactory = LineFactoryPrx::checkedCast(baseLine);
        if (!lineFactory)
            throw "Invalid proxy";
        LinePrx line = lineFactory->createLine("Linia Brzegu 2137");
        cout << "Stworzył się line\n";
        cout << line->getName() << endl;

        Ice::ObjectPrx baseStop = ic->stringToProxy("StopFactory:default -p 10001");
        StopFactoryPrx stopFactory = StopFactoryPrx::checkedCast(baseStop);
        if (!stopFactory)
            throw "Invalid proxy";
        StopPrx stop = stopFactory->createStop("Stop Boks");
        cout << "Stworzył się stop\n";
        cout << stop->getName() << endl;
*/
    } catch (const Ice::Exception& ex) {

        cerr << "Exception: " << ex << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << "Msg:" << msg << endl;
        status = 1;
    }
    if (ic)
        ic->destroy();
    return status;
}