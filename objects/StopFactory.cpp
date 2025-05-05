#include <Ice/Ice.h>
#include "SIPI.h"

StopPrx
StopFactoryI::createStop(const string &name, const Ice::Current &current) {
    try {
        Ice::ObjectPtr servant = new StopI(name);
        current.adapter->add(servant, Ice::stringToIdentity(name));
        cout << "Creating stop: " << name << endl;
        return Ice::uncheckedCast<StopPrx>(
            current.adapter->createProxy(Ice::stringToIdentity(name))
        );
    } catch (const Ice::AlreadyRegisteredException &) {
        throw StopExists{};
    }
}

double
StopFactoryI::getLoad(const Ice::Current &current) {
    return 3.23;
}


int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter stop factory configuration (stop_factory_name/host/port): ";
    string input;
    cin >> input;

    size_t first_slash = input.find('/');
    size_t second_slash = input.find('/', first_slash + 1);

    string stop_factory_name = input.substr(0, first_slash);
    string host = input.substr(first_slash + 1, second_slash - first_slash - 1);
    string port = input.substr(second_slash + 1);

    cout << "Creating stop factory: " << stop_factory_name
         << " on host " << host << " on port " << port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("StopFactoryAdapter", "tcp -h " + host + " -p " + port);
        Ice::ObjectPtr object = new StopFactoryI;
        adapter->add(object, Ice::stringToIdentity("StopFactory"));
        adapter->activate();
        cout << "Stop Factory object created." << endl;

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
