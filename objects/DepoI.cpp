#include <Ice/Ice.h>
#include "SIPI.h"

void
DepoI::TramOnline(const TramPrx &tram,
                  const Ice::Current &current) {
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
DepoI::TramOffline(const TramPrx &tram,
                   const Ice::Current &current) {
    auto it = std::remove_if(online_trams_.begin(), online_trams_.end(),
                             [&tram](const TramInfo &tramInfo) {
                                 return tramInfo.tram->getStockNumber() == tram->getStockNumber();
                             });
    if (it != online_trams_.end()) {
        online_trams_.erase(it, online_trams_.end());
    }
}

void
DepoI::showRegisteredTrams(const Ice::Current &current) {
    TramList trams = online_trams_;
    cout << "\n----------------" << endl;
    cout << "Registered trams:" << endl;
    cout << "----------------" << endl;
    for (const TramInfo &tramInfo: trams) {
        cout << "| " << tramInfo.tram->getStockNumber() << " |" << endl;
    }
    cout << "----------------" << endl;
}

string
DepoI::getName(const Ice::Current &current) {
    return name_;
}

TramList
DepoI::getOnlineTrams(const Ice::Current &current) {
    return online_trams_;
}

TramPrx
DepoI::getTram(const string &stock_number,
                const Ice::Current &current) {
    for (const TramInfo &tramInfo: online_trams_) {
        if (tramInfo.tram->getStockNumber() == stock_number) {
            return tramInfo.tram;
        }
    }
    return nullptr;
}
