#include <Ice/Ice.h>
#include "SIPI.h"

StopPrx
StopFactoryI::createStop(const string& name, const Ice::Current& current)
{
    try {
        Ice::ObjectPtr servant = new StopI(name);
        current.adapter->add(servant, Ice::stringToIdentity(name));
        return Ice::uncheckedCast<StopPrx>(
            current.adapter->createProxy(Ice::stringToIdentity(name))
        );
    } catch (const Ice::AlreadyRegisteredException &) {
        throw StopExists{};
    }
    return nullptr;
}

double
StopFactoryI::getLoad(const Ice::Current& current)
{
    return 3.23;
}


int
main(int argc, char *argv[]) {
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("StopFactoryAdapter", "default -p 10001");
        Ice::ObjectPtr object = new StopFactoryI;
        adapter->add(object, Ice::stringToIdentity("StopFactory"));
        adapter->activate();
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
