module SIP {

  interface Tram;

  interface TramStop;

  interface Depo;

  interface Passenger;

  interface Stop;

  interface Line;

  interface Stop {
    string getName();
  };

  exception LineExists {};
  exception StopExists {};

  struct Time {
    int hour;
    int minute;
  };

  //Informacja -> ten przystanek o tej godzinie
  struct StopInfo
  {
     Time time;
     TramStop* stop;
  };

  struct TramInfo {
     Time time;
     Tram* tram;
  };

  struct DepoInfo {
     string name;
     Depo* stop;
  };


    sequence<TramStop*> TramStopList;
    sequence <Passenger*> PassengerList;
    sequence<StopInfo> StopList;
    sequence<TramInfo> TramList;
    sequence<DepoInfo> DepoList;
    sequence<Line*> LineList;
    sequence<Tram*> TramPrxList;



  interface TramStop {
     string getName();
     TramList getNextTrams(int howMany);
     void setNextTrams(TramList trams);
     void RegisterPassenger(Passenger* p);
     void UnregisterPassenger(Passenger* p);
     void updatePassengerInfo(Tram* t);
  }

  interface Line
  {
		TramPrxList getTrams();
		TramStopList getStops();
		TramStop* getStop(string name);
		void registerTram(Tram* tram);
		void unregisterTram(Tram* tram);
		void setStops(TramStopList sl);
		string getName();
  };

  interface LineFactory {
		Line* createLine(string name);
		double getLoad();
  };

  interface StopFactory {
		Stop* createStop(string name);
		double getLoad();
  };

  interface MPK {
    TramStop* getTramStop(string name);
    TramStopList getTramStops();
    void registerDepo(Depo* depo);
    void unregisterDepo(Depo* depo);
    Depo* getDepo(string name);
    DepoList getDepos();
    LineList getLines();
    Line* getLine(string name);
    void addLine(Line* l);
    void removeLine(Line* l);
    void setStopsForLine(Line* l, string filename);
    void addStop(Stop* st);
    void registerLineFactory(LineFactory* lf);
    void unregisterLineFactory(LineFactory* lf);
    void registerStopFactory(StopFactory* lf);
    void unregisterStopFactory(StopFactory* lf);
    void registerTramStop(TramStop* ts);
  };

  interface Depo {
      void TramOnline(Tram* t);
      void TramOffline(Tram* t);
      void showRegisteredTrams();
      string getName();
      TramList getOnlineTrams();
      Tram* getTram(string stockNumber);
  };

  interface Tram {
    TramStop* getLocation();
    void setLocation(TramStop* ts);
    Line* getLine();
    void setLine(Line* line);
    StopList getNextStops(int howMany);
    void RegisterPassenger(Passenger* p);
    void UnregisterPassenger(Passenger* p);
    void updatePassengerInfo(Tram* tram);
    string getStockNumber();
    void setDepo(Depo* depo);
    StopList getSchedule();
    void setSchedule(StopList sl);
    void updateSchedule(int interval);
    void setTram(Tram* tram);
  };

  interface Passenger
  {
	  void updateTramInfo(Tram* tram, StopList stops);
	  void updateTramStopInfo(Tram* tram);
	  TramStop* getTramStop();
	  void setTramStop(TramStop* ts);
	  void setTram(Tram* tram);
	  Tram* getTram();
  };

};