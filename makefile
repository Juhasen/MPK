.PHONY: all clean
all: slice line_factory system tram passenger

slice:
	slice2cpp SIP.ice

system: objects/system.cpp objects/MPKI.cpp objects/TramStopI.cpp objects/DepoI.cpp
	g++ -I. objects/system.cpp objects/MPKI.cpp objects/TramStopI.cpp objects/DepoI.cpp  SIP.cpp -lIce -lpthread -o system

line_factory: objects/LineI.cpp objects/LineFactory.cpp
	g++ -I. objects/LineFactory.cpp objects/LineI.cpp SIP.cpp -lIce -lpthread -o line_factory

tram: objects/tram.cpp
	g++ -I. objects/tram.cpp SIP.cpp -lIce -lpthread -o tram

passenger: objects/passenger.cpp
	g++ -I. objects/passenger.cpp SIP.cpp -lIce -lpthread -o passenger

clean:
	-rm SIP.cpp SIP.h line_factory system tram passenger


#stop_factory: objects/StopFactory.cpp objects/Stop.cpp
#	g++ -I. objects/StopFactory.cpp objects/Stop.cpp SIP.cpp -lIce -lpthread -o stop_factory