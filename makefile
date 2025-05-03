.PHONY: all clean
all: slice line_factory stop_factory mpk

slice:
	slice2cpp SIP.ice

mpk: objects/Mpk.cpp
	g++ -I. objects/Mpk.cpp SIP.cpp -lIce -lpthread -o mpk

line_factory: objects/LineFactory.cpp objects/Line.cpp
	g++ -I. objects/LineFactory.cpp objects/Line.cpp SIP.cpp -lIce -lpthread -o line_factory

stop_factory: objects/StopFactory.cpp objects/Stop.cpp
	g++ -I. objects/StopFactory.cpp objects/Stop.cpp SIP.cpp -lIce -lpthread -o stop_factory

#depo: src/DepoI.cpp src/TramI.cpp
#	g++ -I. classes/DepoI.cpp classes/TramI.cpp SIP.cpp -lIce -lpthread -o depo

#mpk: src/System.cpp SIP.cpp
#	g++ -I. src/System.cpp SIP.cpp -lIce -lpthread -o mpk

clean:
	-rm SIP.cpp SIP.h line_factory stop_factory mpk