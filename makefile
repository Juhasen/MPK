.PHONY: all clean
all: slice line_factory stop_factory mpk depo tram tram_stop passenger

slice:
	slice2cpp SIP.ice

mpk: objects/Mpk.cpp
	g++ -I. objects/Mpk.cpp SIP.cpp -lIce -lpthread -o mpk

line_factory: objects/LineFactory.cpp objects/Line.cpp
	g++ -I. objects/LineFactory.cpp objects/Line.cpp SIP.cpp -lIce -lpthread -o line_factory

stop_factory: objects/StopFactory.cpp objects/Stop.cpp
	g++ -I. objects/StopFactory.cpp objects/Stop.cpp SIP.cpp -lIce -lpthread -o stop_factory

depo: objects/Depo.cpp
	g++ -I. objects/Depo.cpp SIP.cpp -lIce -lpthread -o depo

tram: objects/Tram.cpp
	g++ -I. objects/Tram.cpp SIP.cpp -lIce -lpthread -o tram

tram_stop: objects/TramStop.cpp
	g++ -I. objects/TramStop.cpp SIP.cpp -lIce -lpthread -o tram_stop

passenger: objects/Passenger.cpp
	g++ -I. objects/Passenger.cpp SIP.cpp -lIce -lpthread -o passenger

clean:
	-rm SIP.cpp SIP.h line_factory stop_factory mpk depo tram tram_stop passenger