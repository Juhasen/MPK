#include <Ice/Ice.h>
#include "SIPI.h"

TramStopPrx
MPKI::getTramStop(const string &name,
                  const Ice::Current &current) {
    return nullptr;
}

void
MPKI::registerDepo(const DepoPrx &depo,
                   const Ice::Current &current) {
}

void
MPKI::unregisterDepo(const DepoPrx &depo,
                     const Ice::Current &current) {
}

DepoPrx
MPKI::getDepo(const string &name,
              const Ice::Current &current) {
    for (DepoInfo depo_info : depos_) {
        if (depo_info.name == name) {
            return depo_info.stop;
        }
    }
    return nullptr;
}

DepoList
MPKI::getDepos(const Ice::Current &current) {
    return depos_;
}

LineList
MPKI::getLines(const Ice::Current &current) {
    return lines_;
}

void
MPKI::registerLineFactory(const LineFactoryPrx &lf,
                          const Ice::Current &current) {
    line_factory_ = lf;
}

void
MPKI::unregisterLineFactory(const LineFactoryPrx &lf,
                            const Ice::Current &current) {
    line_factory_ = nullptr;
}

void
MPKI::registerStopFactory(const StopFactoryPrx &sf,
                          const Ice::Current &current) {
    stop_factory_ = sf;
}

void
MPKI::unregisterStopFactory(const StopFactoryPrx &sf,
                            const Ice::Current &current) {
    stop_factory_ = nullptr;
}

void MPKI::addLine(const LinePrx &line_prx, const Ice::Current &current) {
    lines_.push_back(line_prx);
}

void MPKI::addStop(const StopPrx &stop_prx, const Ice::Current &current) {
    stops_.push_back(stop_prx);
}


void showMenu() {
    cout << "\nMENU:" << endl;
    cout << "1. Create line." << endl;
    cout << "2. Create stop." << endl;
    cout << "3. Show lines." << endl;
}

int
main(int argc, char *argv[]) {
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
                ic->createObjectAdapterWithEndpoints("MpkAdapter", "default -p 12345");
        Ice::ObjectPtr object = new MPKI;
        adapter->add(object, Ice::stringToIdentity("Mpk"));
        adapter->activate();

        cout << "Creating MPK server..." << endl;

        //Register line and stop factories
        Ice::ObjectPrx baseStop = ic->stringToProxy("StopFactory:default -p 10001");
        StopFactoryPrx stopFactory = StopFactoryPrx::checkedCast(baseStop);
        if (!stopFactory)
            throw "Invalid proxy";

        Ice::ObjectPrx baseLine = ic->stringToProxy("LineFactory:default -p 10000");
        LineFactoryPrx lineFactory = LineFactoryPrx::checkedCast(baseLine);
        if (!lineFactory)
            throw "Invalid proxy";

        MPKPrx mpk = Ice::uncheckedCast<MPKPrx>(adapter->createProxy(Ice::stringToIdentity("Mpk")));
        mpk->registerLineFactory(lineFactory);
        mpk->registerStopFactory(stopFactory);

        while (true) {
            showMenu();
            char choice = '0';
            cin >> choice;
            switch (choice) {
                case '1': {
                    string line_name;
                    cout << "Enter line name: ";
                    cin >> line_name;
                    LinePrx line = lineFactory->createLine(line_name);
                    mpk->addLine(line);
                    break;
                }
                case '2': {
                    string stop_name;
                    cout << "Enter stop name: ";
                    cin >> stop_name;
                    StopPrx stop = stopFactory->createStop(stop_name);
                    mpk->addStop(stop);
                    break;
                }
                case '3': {
                    LineList lines = mpk->getLines();
                    cout << "\n----------------" << endl;
                    cout << "Dostępne linie:" << endl;
                    cout << "----------------" << endl;
                    for (LinePrx currLine : lines) {
                        cout << "| " << currLine.get()->getName() << " |" << endl;
                    }
                    cout << "----------------" << endl;
                    break;
                }
                default: {
                    cout << "Bad choice!" << endl;
                    break;
                }
            }
        }
    } catch (const Ice::Exception &ex) {
        cerr << "Exception: " << ex << endl;
        status = 1;
    } catch (const char *msg) {
        cerr << "Msg:" << msg << endl;
        status = 1;
    }
    if (ic)
        ic->destroy();
    return status;
}
