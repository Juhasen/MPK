#include <Ice/Ice.h>
#include "SIPI.h"
 
int
main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);


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