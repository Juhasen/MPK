module SIP {

  interface Tram;

  interface TramStop;

  interface Depo;

  interface Passenger;

  interface Stop;

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

  sequence<StopInfo> StopList;

  //
  struct TramInfo {
     Time time;
     Tram* tram;
  };

  sequence<TramInfo> TramList;

  struct DepoInfo {
     string name;
     Depo* stop;
  };

  sequence<DepoInfo> DepoList;

  interface TramStop {
     string getName();
     TramList getNextTrams(int howMany);
     void RegisterPassenger(Passenger* p);
     void UnregisterPassenger(Passenger* p);
     void UpdateTramInfo(Tram* tram, Time time);
  }

  interface Line
  {
		TramList getTrams();
		StopList getStops();
		TramStop* getStop(string name);
		void registerTram(Tram* tram);
		void unregisterTram(Tram* tram);
		void setStops(StopList sl);
		string getName();
  };

  sequence<Line*> LineList;

  interface LineFactory {
		Line* createLine(string name);
		double getLoad();
  };

  interface StopFactory {
		Stop* createStop(string name);
		double getLoad();
  };

  sequence<TramStop*> TramStopList;

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
  };

  sequence <Passenger*> PassengerList;

  interface Passenger
  {
	  void updateTramInfo(Tram* tram, StopList stops);
	  void updateStopInfo(Stop* stop, TramList trams);
  };

};