#ifndef __SIPI_h__
#define __SIPI_h__

#include <iostream>
#include <string>
#include <cstring>      // memset, strerror
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>     // close
#include <errno.h>
#include <memory>
#include <thread>
#include <chrono>
#include <fstream>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <Ice/Ice.h>
#include "../SIP.h"

using namespace SIP;
using namespace std;

class StopI : public virtual Stop {
public:
    StopI(const string &name) : name_(name) {
    }

    virtual string getName(const Ice::Current &) override;

private:
    string name_;
};

class TramStopI : public virtual TramStop {
public:
    explicit TramStopI(const string &name) : name_(name) {
    }

    virtual string getName(const Ice::Current &) override;

    virtual TramList getNextTrams(int,
                                  const Ice::Current &) override;

    virtual void RegisterPassenger(const PassengerPrx &passenger_prx,
                                   const Ice::Current &) override;

    virtual void UnregisterPassenger(const PassengerPrx &passenger_prx,
                                     const Ice::Current &) override;

    virtual void updatePassengerInfo(const TramPrx &tram, const Ice::Current &) override;

    virtual void setNextTrams(const TramList &next_trams,
                              const Ice::Current &current) override;

private:
    string name_;
    TramList next_trams_;
    PassengerList passengers_;
};

class LineI : public virtual Line {
public:
    LineI(const string &name) : name_(name) {
    }

    virtual TramPrxList getTrams(const Ice::Current &) override;

    virtual TramStopList getStops(const Ice::Current &) override;

    virtual TramStopPrx getStop(const string &name,
                                const Ice::Current &) override;

    virtual void registerTram(const TramPrx &tram,
                              const Ice::Current &) override;

    virtual void unregisterTram(const TramPrx &tram,
                                const Ice::Current &) override;

    virtual void setStops(const TramStopList &stop_list,
                          const Ice::Current &) override;

    virtual string getName(const Ice::Current &) override;

private:
    string name_;
    TramStopList stops_;
    TramPrxList trams_;
};

class LineFactoryI : public virtual LineFactory {
public:
    virtual LinePrx createLine(const string &name,
                               const Ice::Current &) override;

    virtual double getLoad(const Ice::Current &) override;

private:
    long load_ = 0;
};

class StopFactoryI : public virtual StopFactory {
public:
    virtual StopPrx createStop(const string &name,
                               const Ice::Current &) override;

    virtual double getLoad(const Ice::Current &) override;
};

class MPKI : public virtual MPK {
public:
    virtual TramStopPrx getTramStop(const string &name,
                                    const Ice::Current &) override;

    virtual TramStopList getTramStops(const Ice::Current &current) override;

    virtual void registerDepo(const DepoPrx &depo,
                              const Ice::Current &) override;

    virtual void unregisterDepo(const DepoPrx &depo,
                                const Ice::Current &) override;

    virtual DepoPrx getDepo(const string &name,
                            const Ice::Current &) override;

    virtual DepoList getDepos(const Ice::Current &) override;

    virtual LineList getLines(const Ice::Current &) override;

    virtual LinePrx getLine(const string &name,
                            const Ice::Current &) override;

    virtual void removeLine(const LinePrx &line, const Ice::Current &current) override;

    virtual void registerLineFactory(const LineFactoryPrx &line_factory_prx,
                                     const Ice::Current &) override;

    virtual void unregisterLineFactory(const LineFactoryPrx &line_factory_prx,
                                       const Ice::Current &) override;

    virtual void registerStopFactory(const StopFactoryPrx &stop_factory_prx,
                                     const Ice::Current &) override;

    virtual void unregisterStopFactory(const StopFactoryPrx &stop_factory_prx,
                                       const Ice::Current &) override;

    virtual void addLine(const LinePrx &line,
                         const Ice::Current &) override;

    virtual void setStopsForLine(const LinePrx &line_prx, const string &filename, const Ice::Current &current) override;

    virtual void addStop(const StopPrx &stop,
                         const Ice::Current &) override;

    virtual void registerTramStop(const TramStopPrx &tram_stop_prx,
                                  const Ice::Current &) override;

private:
    LineList lines_;
    vector<TramStopPrx> tram_stops_;
    vector<StopPrx> stops_;
    DepoList depos_;
    LineFactoryPrx line_factory_;
    StopFactoryPrx stop_factory_;
};

class DepoI : public virtual Depo {
public:
    explicit DepoI(const string &name) : name_(name) {
    }

    virtual void TramOnline(const TramPrx &tram_prx,
                            const Ice::Current &) override;

    virtual void TramOffline(const TramPrx &tram_prx,
                             const Ice::Current &) override;

    virtual string getName(const Ice::Current &) override;

    virtual TramList getOnlineTrams(const Ice::Current &current) override;

    virtual TramPrx getTram(const string &stock_number,
                            const Ice::Current &) override;

    virtual void showRegisteredTrams(const Ice::Current &) override;

private:
    string name_;
    TramList online_trams_;
};

class TramI : public virtual Tram {
public:
    explicit TramI(const string &stock_number) : stock_number_(stock_number) {
    }

    virtual TramStopPrx getLocation(const Ice::Current &) override;

    virtual void setLocation(const TramStopPrx &location_prx,
                             const Ice::Current &) override;

    virtual LinePrx getLine(const Ice::Current &) override;

    virtual void setLine(const LinePrx &line_prx,
                         const Ice::Current &) override;

    virtual StopList getNextStops(int howMany,
                                  const Ice::Current &) override;

    virtual void RegisterPassenger(const PassengerPrx &passenger_prx,
                                   const Ice::Current &) override;

    virtual void UnregisterPassenger(const PassengerPrx &passenger_prx,
                                     const Ice::Current &) override;

    virtual void updatePassengerInfo(const TramPrx &tram,
                                     const Ice::Current &) override;

    virtual string getStockNumber(const Ice::Current &) override;

    virtual void setDepo(const DepoPrx &depo, const Ice::Current &) override;

    virtual StopList getSchedule(const Ice::Current &) override;

    virtual void setSchedule(const StopList &schedule,
                             const Ice::Current &) override;

    virtual void updateSchedule(int interval,
                                const Ice::Current &) override;

    virtual void setTram(const TramPrx &tram, const Ice::Current &) override;

private:
    string stock_number_;
    TramPrx tram_;
    DepoPrx depo_;
    LinePrx line_;
    StopList schedule_;
    TramStopPrx location_;
    PassengerList passengers_;
};

class PassengerI : public virtual Passenger {
public:
    virtual void updateTramInfo(const TramPrx &tram_prx,
                                const StopList &stop_list,
                                const Ice::Current &) override;

    virtual void updateTramStopInfo(const TramPrx &tram_prx,
                                const Ice::Current &) override;

    virtual void setTram(const TramPrx &tram_prx, const Ice::Current &) override;

    virtual void setTramStop(const TramStopPrx &stop_prx, const Ice::Current &) override;

    virtual TramPrx getTram(const Ice::Current &) override;

    virtual TramStopPrx getTramStop(const Ice::Current &) override;

private:
    TramPrx tram_;
    TramStopPrx stop_;
};


inline string getNetworkInterface() {
    ifaddrs *interfaces = nullptr, *ifa = nullptr;
    vector<pair<string, string> > interfaceList;

    if (getifaddrs(&interfaces) == -1) {
        perror("getifaddrs");
        return "";
    }

    // Collect and display all network interfaces with IPv4 addresses
    cout << "Available network interfaces:\n";
    int index = 1;
    for (ifa = interfaces; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            // IPv4 only
            char addressBuffer[INET_ADDRSTRLEN];
            void *addrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, addrPtr, addressBuffer, INET_ADDRSTRLEN);

            string name(ifa->ifa_name);
            string ip(addressBuffer);
            interfaceList.emplace_back(name, ip);

            cout << index << ") " << name << " : " << ip << "\n";
            ++index;
        }
    }

    if (interfaceList.empty()) {
        cerr << "No IPv4 interfaces found.\n";
        freeifaddrs(interfaces);
        return "";
    }

    int choice = 0;
    cout << "\nEnter the number of the interface to select: ";
    cin >> choice;

    freeifaddrs(interfaces);

    if (choice < 1 || choice > static_cast<int>(interfaceList.size())) {
        cerr << "Invalid choice.\n";
        return "";
    }

    const auto &[name, ip] = interfaceList[choice - 1];
    return ip;
}

inline bool isPortBusy(const string &host, const string &port) {
    addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (status != 0) {
        cerr << "getaddrinfo: " << gai_strerror(status) << "\n";
        return false;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        cerr << "socket: " << strerror(errno) << "\n";
        freeaddrinfo(res);
        return false;
    }

    status = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if (status == -1) {
        close(sockfd);
        freeaddrinfo(res);
        return false;
    }

    close(sockfd);
    freeaddrinfo(res);
    return true;
}
#endif
