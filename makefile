.PHONY: all clean
all: slice line_factory stop_factory client

slice:
	slice2cpp --impl-c++11 SIP.ice

line_factory: objects/LineFactory.cpp objects/Line.cpp
	g++ -I. objects/LineFactory.cpp objects/Line.cpp SIP.cpp -lIce -lpthread -o line_factory

client: objects/client.cpp
	g++ -I. objects/client.cpp SIP.cpp -lIce -lpthread -o client

stop_factory: objects/StopFactory.cpp objects/Stop.cpp
	g++ -I. objects/StopFactory.cpp objects/Stop.cpp SIP.cpp -lIce -lpthread -o stop_factory

#depo: src/DepoI.cpp src/TramI.cpp
#	g++ -I. classes/DepoI.cpp classes/TramI.cpp SIP.cpp -lIce -lpthread -o depo

#mpk: src/System.cpp SIP.cpp
#	g++ -I. src/System.cpp SIP.cpp -lIce -lpthread -o mpk

clean:
	-rm SIPI.cpp SIPI.h line_factory stop_factory client