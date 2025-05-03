#include <Ice/Ice.h>
#include "SIPI.h"

void
DepoI::TramOnline(const TramPrx& tram,
                       const Ice::Current& current)
{
    auto now = chrono::system_clock::now();
    time_t now_time_t = chrono::system_clock::to_time_t(now);

    tm local_time{};
    localtime_r(&now_time_t, &local_time);

    TramInfo tramInfo;
    tramInfo.tram = tram;
    tramInfo.time.hour = local_time.tm_hour;
    tramInfo.time.minute = local_time.tm_min;

    online_trams_.push_back(tramInfo);
}

void
DepoI::TramOffline(const TramPrx& tram,
                        const Ice::Current& current)
{
    auto it = std::remove_if(online_trams_.begin(), online_trams_.end(),
                             [&tram](const TramInfo& tramInfo) {
                                 return tramInfo.tram->getStockNumber() == tram->getStockNumber();
                             });
    if (it != online_trams_.end()) {
        online_trams_.erase(it, online_trams_.end());
    }
    cout << "Tram unregistered." << endl;
}

string
DepoI::getName(const Ice::Current& current)
{
    return name_;
}

int
main(int argc, char* argv[]) {

}