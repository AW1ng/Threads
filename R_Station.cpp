#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;

class Poezd;

class Vokzal {
	protected:
		string Name;
		string TrainID;
		mutex locker;
	public:
		void Arrive( Poezd *train );
		void Depart() {
			TrainID = "None";
		}
		Vokzal(string name) {
			cout << "Vokzal is C-tor is here\n";
			TrainID = "None";
			Name = name;
		}	
		~Vokzal() {
			cout << "Vokzal D-tor is here\n";
		}
		
		string GetName() { return Name; }
		
		string ToStr() {
			if( TrainID == "None" ) return Name + " No train here";
			else return "Here is the train ID " + TrainID;
		}
};


class Poezd {
	protected:
		int WayTime, StayTime;
		Vokzal *Destination;
		string Name;
		public:
			string GetName() {
				return Name;
			}
			int GetStayTime() {
				return StayTime;
			}
			
			Poezd( string name, Vokzal *destination, int wayTime, int stayTime ){
				cout << " Poezd C-tor is here\n";
				Name = name; WayTime = wayTime; StayTime = stayTime;
				Destination = destination;
			}
			~Poezd() {
				cout << "Poezd D-tor is here\n";
			}
			string ToStr() {
				return "Name: " + Name + " Destination " + Destination->GetName() +\
				 " Waytime: " + to_string(WayTime) + " Stop for: " + to_string(StayTime);
			}
			void Go( ) {
				cout << Name + " waiting for " + to_string(WayTime) + " for arrive\n"; 
				this_thread::sleep_for( std::chrono::seconds( WayTime ) );
				cout << Name + " arriving to " + Destination->GetName() + "\n"; 
				Destination->Arrive( this );
//				cout << Name + " arrived to " + Destination->GetName() + "\n"; 
//				cout << Name + " waiting " + to_string(StayTime) + " for depart\n";
//				this_thread::sleep_for( std::chrono::seconds( StayTime ) );
//				cout << Name + " departing from " + Destination->GetName() + "\n"; 
//				Destination->Depart();
				cout << Name + " departed from " + Destination->GetName() + '\n';
			}
};

void Vokzal::Arrive( Poezd *train) {
		locker.lock();
		TrainID = train->GetName();
		this_thread::sleep_for( std::chrono::seconds( train->GetStayTime() ) );
		Depart();
		locker.unlock();
} 

void threadFunction( Poezd **trains, int trainNum) {
	trains[trainNum]->Go();
}

void reporterFunction(Vokzal *station, int period, int maxiter) {
	for( int i = 0; i < maxiter; i++) {
		cout << "Report " + to_string(i) + ": " + station->ToStr() + "\n";
		this_thread::sleep_for( chrono::seconds(period) );
	}
}

class Doroga {
	protected:
		Vokzal *Station;
		vector<Poezd*> Trains;
		vector<thread> Thrds;
		
		void ReporterFunction(Vokzal *station, int period, int maxiter) {
			for( int i = 0; i < maxiter; i++) {
			cout << "Report " + to_string(i) + ": " + station->ToStr() + "\n";
			this_thread::sleep_for( chrono::milliseconds(period) );
			}
		}
		
		std::thread ThreadForReporter( Vokzal *station, int period, int maxiter) {
			return std::thread( &Doroga::ReporterFunction, this, station, period, maxiter );
		}
		
		std::thread ThreadForPoezdGo( int TrainID ) {
			return std::thread( &Poezd::Go, Trains[ TrainID ] );
		}
//		void ThreadFunction( int trainNum ) {
//			Trains[trainNum]->Go();
//		}
		
	public:
		Doroga( string VokzalName, int Period = 300, int MaxIter = 20) {
			Station = new Vokzal(VokzalName);
			Trains.clear();
			Thrds.clear();
			Thrds.push_back( ThreadForReporter( Station, Period, MaxIter) );
		}
		
		~Doroga() { 
//			Reporter->join();
			for( int i = 0; i < Thrds.size(); i++ ) {
				Thrds[i].join();
			}
			delete Station;
			Thrds.clear();
			
		 }
		 
		void AddTrain( string TrainName,int Waytime , int StayTime ) {
			Trains.push_back( new Poezd( TrainName, Station, Waytime, StayTime ) );
			Thrds.push_back( ThreadForPoezdGo( Trains.size() - 1) );
		}
};



int main() {
	
	Doroga *RailWay = new Doroga( "Moskva", 500, 40);
	RailWay->AddTrain("9123", 3, 5);
	RailWay->AddTrain("1502", 2, 4);
	RailWay->AddTrain("LA", 2, 4);
	RailWay->AddTrain("IVO", 4, 2);
//	Vokzal *Station = new Vokzal( "Moskva" );
//	cout << Station->ToStr();
//	Poezd *Trains[2];
//	Trains[0] = new Poezd ("JS", Station, 3, 4);
//	Trains[1] = new Poezd ("FC", Station, 1, 3);
//	cout << Trains[0]->ToStr() + "\n";
//	cout << Trains[1]->ToStr() + "\n";
//	thread thr0( threadFunction,Trains, 0);
//	thread thr1( threadFunction, Trains, 1);
//	thread thr2( reporterFunction, Station, 1, 10 );
//	thr0.join();
//	thr1.join();
//	thr2.join();
////	Trains[0]->Go();
////	Trains[1]->Go();
//	delete Station;
//	delete Trains[0];
//	delete Trains[1];
delete RailWay;

}

