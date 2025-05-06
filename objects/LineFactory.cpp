#include <Ice/Ice.h>
#include "SIPI.h"

LinePrx
LineFactoryI::createLine(const string &name, const Ice::Current &current) {
    try {
        Ice::ObjectPtr servant = new LineI(name);
        current.adapter->add(servant, Ice::stringToIdentity(name));
        cout << "Creating line: " << name << endl;
        load_++;
        return Ice::uncheckedCast<LinePrx>(
            current.adapter->createProxy(Ice::stringToIdentity(name))
        );
    } catch (const Ice::AlreadyRegisteredException &) {
        throw LineExists{};
    }
}

double
LineFactoryI::getLoad(const Ice::Current &current) {
    return load_;
}


int
main(int argc, char *argv[]) {
    int status = 0;
    cout << "Enter LineFactory configuration (LineFactoryName/port): ";
    string input;
    cin >> input;
    size_t first_slash = input.find('/');
    string line_factory_name = input.substr(0, first_slash);
    string port = input.substr(first_slash + 1);
    string host = getNetworkInterface();
    if (!isPortBusy(host, port)) {
        cerr << "Port " << port << " is not available." << endl;
        return 1;
    }
    cout << "Creating LineFactory: " << line_factory_name << " on host " << host << " on port " << port << endl;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("LineFactoryAdapter", "tcp -h " + host + " -p " + port);
        Ice::ObjectPtr object = new LineFactoryI;
        adapter->add(object, Ice::stringToIdentity(line_factory_name));
        adapter->activate();
        cout << "Line Factory object created." << endl;

        ic->waitForShutdown();
    }
    catch (const Ice::EndpointParseException &ex) {
        cerr << "Błąd parsowania endpointów: " << ex.what() << endl;
        return 1;
    }
    catch (const Ice::Exception &e) {
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
