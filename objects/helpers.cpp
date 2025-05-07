#include "SIPI.h"
#include <mutex>

void monitorMPKConnection(const MPKPrx& mpk, const Ice::CommunicatorPtr& ic) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        try {
            mpk->ice_ping();
        } catch (const Ice::Exception& e) {
            std::cerr << "Utracono połączenie z serwerem MPK: " << e.what() << std::endl;
            ic->shutdown();
            break;
        }
    }
}

