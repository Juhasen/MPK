#include <Ice/Ice.h>
#include "SIPI.h"

LinePrx
LineFactoryI::createLine(const string &name, const Ice::Current &current) {
    try {
        Ice::ObjectPtr servant = new LineI(name);
        current.adapter->add(servant, Ice::stringToIdentity(name));
        return Ice::uncheckedCast<LinePrx>(
            current.adapter->createProxy(Ice::stringToIdentity(name))
        );
    } catch (const Ice::AlreadyRegisteredException &) {
        throw LineExists{};
    }
    return nullptr;
}

double
LineFactoryI::getLoad(const Ice::Current &current) {
    return 2.32;
}


int
main(int argc, char *argv[]) {
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("LineFactoryAdapter", "default -p 10000");
        Ice::ObjectPtr object = new LineFactoryI;
        adapter->add(object, Ice::stringToIdentity("LineFactory"));
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
