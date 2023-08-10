#include <iostream>
#include <cstdlib> 
#include <ctime> 
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <queue>
#include <list>

using namespace std;


int GoFarAway( int destination, int speed = 1 ) { 
	return ( destination  + 1 ) * speed;
} 


int CookSushi(int amount = 1 , string Sort = "Filadelfia", int latency = 200 ) {
    for( int i = 0; i < amount; i++) {
        cout<<to_string(i)+": Cooking sushi "+Sort+"...";
        this_thread::sleep_for(chrono::milliseconds(latency));
        cout<<"ready\n";
        }
    return amount*latency;
    }

int CookPizza(int amount = 1 , string Sort = "Margarita", int latency = 300 ) {
    for( int i = 0; i < amount; i++) {
        cout<<to_string(i)+": Cooking pizza "+Sort+"...";
        this_thread::sleep_for(chrono::milliseconds(latency));
        cout<<"ready\n";
        }
    return amount*latency;
    }

int CookSteak(int amount = 1 ) { return CookPizza( amount, "Ribai", 150); }
int CookSushi(int amount = 1 ) { return CookSushi( amount, "Filadelfia", 200); }
int CookSalad(int amount = 1 ) { return CookSushi( amount, "Ceazar", 100); }
int CookSoup(int amount = 1 ) { return CookSushi( amount, "Lagman", 300); }
int CookPizza(int amount = 1 ) { return CookPizza( amount, "Margarita", 250); }

class Dish {
    protected:
        string Name;
        int Amount;
        int ClientID;
    public:
        string GetName() { return Name; }
        int GetAmount() { return Amount; }
        int GetClientID() { return ClientID; }
        Dish(string name = "Water", int amount = 1, int ClientId = 0 ) {
            //cout<<"Dish C-tor is here\n";
            ClientID = ClientId;
            Name = name;
            Amount = amount;
            }
        ~Dish() { /*cout<<"Dish D-tor is here\n"*/ ; }
        virtual string ToStr() { return "Dish: "+Name+" "+to_string(Amount)+" for "+to_string(ClientID); }
    }; 

class ReadyDish : public Dish {
    protected:
        int Weight;
    public:
        void SetWeight(int w) { Weight = w; }
        ReadyDish( int weight = 0 ) : Dish() { SetWeight( weight ); }
        ReadyDish( Dish Ordered, int weight = 100 ) : Dish( Ordered.GetName(), Ordered.GetAmount(), Ordered.GetClientID() ) { SetWeight(weight); }
        string ToStr() { return Dish::ToStr() + ", " + to_string( Weight ) + " gramms."; }
        
    };

template <typename T> // Middle-class between
class ProtectedStorage {
    protected:
        queue<T> data;
        std::recursive_mutex _lock;
    public:
        ProtectedStorage() {
            cout<<"PStorage constructor is here\n";
            while( !data.empty() ) data.pop();
            }
        ~ProtectedStorage() {
            cout<<"PStorage destructor is here\n";
            while( !data.empty() ) data.pop();
            }
        bool Get( T &result ) {
            _lock.lock();
            if( !IsEmpty() ) {
                result = data.front();
                data.pop();
                _lock.unlock();
                return true;
                }
            else {
            	_lock.unlock(); return false;
			}
        }
        void Put(T item) {
//		 cout << "Putting" + item.ToStr() + "\n";
		 _lock.lock(); data.push(item); _lock.unlock(); 
		 }
        bool IsEmpty() { return data.empty(); }
        void Print() {
        	string toScreen = " ";
        	if( !data.size() ) cout << "Storage is empty\n";
            else {
            	_lock.lock();
				for(int i = 0 ; i < data.size() ; i ++ ) {
                	T temp = data.front();
                	data.pop();
                	toScreen += to_string(i+1)+". "+temp.ToStr()+"\n";
                	data.push(temp);
                }
                _lock.unlock();
                cout << toScreen + "\n" << flush;
            	}
            }
        void Report( int Every = 500, int MaxIter = 3 ) {
            for(int i = 0; i < MaxIter; i++) {
                Print();
                this_thread::sleep_for(chrono::milliseconds(Every));
                }
            }
        std::thread ThreadForReport(int every, int maxiter) {
            return std::thread( &ProtectedStorage::Report, this, every, maxiter );
            }
    };

class Cooker {
    protected:
        int SpentTime;
        ProtectedStorage<Dish> *MidOfficiantTable;
        ProtectedStorage<ReadyDish> *MidDeliveryTable;

        map< string, int(*)( int ) > Recipes;
        int InitRecipes() {
            cout<<"Initing recipes...\n";
            Recipes["Sushi"] = &CookSushi;
            Recipes["Pizza"] = &CookPizza;
            Recipes["Steak"] = &CookSteak;
            Recipes["Soup"] = &CookSoup;
            Recipes["Salad"] = &CookSalad;
            return 5;
            }
    public:
        Cooker(ProtectedStorage<Dish> *orders, ProtectedStorage<ReadyDish> *Products ) {
            cout<<"Cooker c-tor is here\n";
            MidOfficiantTable = orders;
            MidDeliveryTable = Products;
            SpentTime = 0;
            InitRecipes();
            }
        ~Cooker() {
            cout<<"Cooker d-tor is here. Total spent time: "+to_string(SpentTime)+"\n";
            }
        int Cook( int(*recipe)(int, string, int) , int amount , string dish, int latency ) {
            int result = (*recipe)(amount, dish, latency);
            SpentTime += result;
            return result;
            }
        int Cook( string recipe, int amount , string dish, int latency ) {
            int result = Recipes[recipe](amount);//, dish, latency);
            SpentTime += result;
            return result;
            }
        int Cook( Dish Order ) {
            int result = Recipes[ Order.GetName() ]( Order.GetAmount() );
            SpentTime += result;
            return result;
            }
        vector<string> GetRecipesList() {
            vector<string> result;
            for( auto & kvp : Recipes )
                result.emplace_back(kvp.first);
            return result;
            }
        bool GetOrder( Dish &order ) {
            bool result  = MidOfficiantTable->Get( order );
            if(result) cout<<order.ToStr()+"\n";
            return result;
            }
        bool Proceed() {
        	Dish gotOrder;
        	if( GetOrder( gotOrder ) ) {
        		int mass = gotOrder.GetClientID() + 10;
        		Cook( gotOrder );
        		ReadyDish Meal( gotOrder, mass);
        		MidDeliveryTable->Put( Meal );
        		return true;
			}
			else return false;
		}
		void ProceedOrders( int MaxIter = 10, int DelayFailure = 100 ) {
			for( int i = 0; i < MaxIter; i++ ) {
				if( !Proceed() ) this_thread::sleep_for(chrono::milliseconds( DelayFailure ) ) ; 
			}
		}
		std::thread ThreadForProceedOrders( int Maxiter = 10, int DelayFailure = 100 ) {
			return std::thread( &Cooker::ProceedOrders, this, Maxiter, DelayFailure );
		} 
    	
    };


class Officiant {
    protected:
        Cooker *Povar;
        ProtectedStorage<Dish> *Buffer;
        int EveryMin, EveryMax, AmountMin, AmountMax;
    public:
        vector<string> GetDishesList() { return Povar->GetRecipesList(); }
        Officiant(Cooker *Povar, ProtectedStorage<Dish> *buffer, int everymin = 100, int everymax = 300, int amountmin = 1, int amountmax = 6) { 
            this->Povar = Povar;
            Buffer = buffer;
            EveryMin = everymin; EveryMax = everymax;
            AmountMin = amountmin; AmountMax = amountmax;
            }
        Dish RandomDish() {
            int rAmount = AmountMin + rand()%(AmountMax - AmountMin + 1);
            vector<string> Dishes = GetDishesList();
            string rDish = Dishes[ rand() % Dishes.size() ];
            int ClientId = rand()%12;
            Dish *result = new Dish( rDish, rAmount, ClientId );
            return *result;
            }
        void PlaceRandomOrder( bool needwait = false ) {
            if( needwait ) {
            	this_thread::sleep_for(chrono::milliseconds( EveryMin + rand()%(EveryMax - EveryMin +1) ));
			}
            Buffer->Put( RandomDish() );
            cout<<"Placed order\n"<<flush;
            }
        void PlaceOrders( int MaxOrders = 5 ) {
        	srand((unsigned)time(0)); // this need for random in each thread 
			for(int i = 0 ; i < MaxOrders ; i++ )
				PlaceRandomOrder( true );
			}
        std::thread ThreadForPlaceOrders( int MaxIter ) {
            return std::thread( &Officiant::PlaceOrders, this, MaxIter );
            }
        //TODO: thread for placing, multiple placing and reporting
    };

class CallCenter {
    protected:
        list<Officiant*> Personal;
        vector<std::thread> Threads;
    public:
        CallCenter(Cooker *povar, ProtectedStorage<Dish> *stolik, int N = 2, int Kontrakt = 2) {
            cout<<"Creating callcenter... ";
            Personal.clear(); Threads.clear();
            for(int i = 0; i < N ; i++) Personal.push_back( new Officiant(povar, stolik) );
            for(auto Pers : Personal) Threads.push_back(Pers->ThreadForPlaceOrders(Kontrakt));
            cout<<"Callcenter created.\n";
            }
        ~CallCenter() {
            cout<<"Callcenter destruction... ";
            for(int i = 0 ; i < Threads.size() ; i++) Threads[i].join();
            for( auto Pers : Personal ) delete Pers;
            Personal.clear(); Threads.clear();
            cout<<"Callcenter destroyed.\n";
            }
    };
    
    class Courier {
    	protected:
    		ProtectedStorage<ReadyDish> *Delivers;
    		int deliverySpeed, SpentTime, ID, Counter;
    		
    		public:
    			Courier(ProtectedStorage<ReadyDish> *DelTable, int id, int delivery_speed = 10 ) {
    				cout << "\nCourier" + to_string( id ) + " C-tor is here.\n";
    				SpentTime = 0; ID = id; Counter = 0;
    				deliverySpeed = delivery_speed; 
    				Delivers = DelTable;
				} 
				~Courier() {
					cout << "\nCourier " + to_string( ID ) + " dies after " + to_string( Counter ) + " orders, spent time: " + to_string(SpentTime) + "\n";
				}
				bool GetOrder( ReadyDish &order ) {
					return Delivers->Get( order );
				}
				
				bool Proceed() { 
					ReadyDish Order;
					if( GetOrder( Order ) ) {
						Counter++;
						SpentTime += GoFarAway( Order.GetClientID(), deliverySpeed );
						cout << "----->" + Order.ToStr() + "\n";
						return true;
					}
					else return false;
				}
				
				void ProceedOrders( int MaxIter = 10, int DelayFailure = 100 ) {
					for( int i = 0; i < MaxIter; i++ ) {
						if( !Proceed() ) {
							cout << "Nothing to carry for " + to_string( ID ) + "\n";
							this_thread::sleep_for(chrono::milliseconds( DelayFailure ) ) ; 
						}
					}
				}
				std::thread ThreadForProceedOrders( int Maxiter = 10, int DelayFailure = 100 ) {
					return std::thread( &Courier::ProceedOrders, this, Maxiter, DelayFailure );
				} 
	};
    
    
    class Kitchen {
    	protected:
    		list<Cooker*> Personal;
    		vector<std::thread> Threads;
    		public:
    		Kitchen( ProtectedStorage<Dish> *InputT, ProtectedStorage<ReadyDish> *OutputT, int NCookers = 1, int MaxIter = 10, int DelayFailure = 100)	{
    			cout << "Creatin kitchen... ";
    			Personal.clear(); Threads.clear();
    			for(int i = 0; i < NCookers ; i++) Personal.push_back( new Cooker(InputT, OutputT) );
            	for(auto Pers : Personal) Threads.push_back(Pers->ThreadForProceedOrders(MaxIter, DelayFailure));
            	cout<<"Kitchen created.\n";
            }
       		 ~Kitchen() {
            	cout<<"Kitchen destruction... ";
            	for(int i = 0 ; i < Threads.size() ; i++) Threads[i].join();
            	for( auto Pers : Personal ) delete Pers;
            	Personal.clear(); Threads.clear();
            	cout<<"Kitchen destroyed.\n";
            }
	};
	
	class Delivery {
		protected:
			vector<Courier*> Personal;
			vector<std::thread> Threads;
		public:
			Delivery( ProtectedStorage<ReadyDish> *OrdersTable, int NCouriers = 1, int MaxIter = 10, int Failure = 100 ) {
				cout << "\nDelivery creation...";
				Personal.clear(); Threads.clear();
    			for(int i = 0; i < NCouriers ; i++) Personal.push_back( new Courier( OrdersTable, i, 20 - 2 * i ) );
            	for(auto Pers : Personal) Threads.push_back(Pers->ThreadForProceedOrders(MaxIter, Failure));
            	cout<<"Delivery created.\n";
			}
			~Delivery() {
            	cout<<"Delivery destruction... ";
            	for(int i = 0 ; i < Threads.size() ; i++) Threads[i].join();
            	for( auto Pers : Personal ) delete Pers;
            	Personal.clear(); Threads.clear();
            	cout<<"Delivery destroyed.\n";
            }
	};
	
	class Restaurant {
		protected:
			ProtectedStorage<Dish> *OffCookTable;
   			ProtectedStorage<ReadyDish> *CookDelTable;
    		Cooker *Povar;
   		 	CallCenter *MyCallCenter;
   		 	thread Reporting;
    		thread ReportingR;
    		Kitchen *MyKitchen; 
    		Delivery *MyDelivery;
		public:
			Restaurant( int OffN = 1, int OffMI = 10, int DelN = 5, int DelMI = 100, int NCers = 1, int CersMI = 10, int OpDelay = 100 ) {
				srand((unsigned)time(0));
				OffCookTable = new ProtectedStorage<Dish>;
    			CookDelTable = new ProtectedStorage<ReadyDish>;
    			Povar = new Cooker(OffCookTable, CookDelTable);
    			MyCallCenter = new CallCenter(Povar, OffCookTable, OffN, OffMI);
    			Reporting = OffCookTable->ThreadForReport(5 * OpDelay, CersMI + OffN*OffMI );
    			ReportingR = CookDelTable->ThreadForReport(5 * OpDelay, 2 * CersMI + OffN * OffMI);
    			MyKitchen = new Kitchen( OffCookTable, CookDelTable, NCers, CersMI, OpDelay); 
    			MyDelivery = new Delivery( CookDelTable, DelN, DelMI, OpDelay );
			}
			~Restaurant() {
				delete MyCallCenter;
				delete MyKitchen;
				delete MyDelivery;
    			Reporting.join(); ReportingR.join();
    			delete Povar;
    			delete OffCookTable; delete CookDelTable;
			}
	};


int main() {
	
	Restaurant *MyRestaurant = new Restaurant;
	delete MyRestaurant;
	
    return 0;
    }

