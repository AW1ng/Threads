#include <iostream>
//#include <pthread.h>
//#include <windows.h>
#include <unistd.h>
#include <vector>
#include <thread>



class SwimmingPool { 
	protected:
		double *Coords;
		double *Vels;
		double PoolLength;
		int NSwimmers;
	public:
		SwimmingPool( int n = 3, double l = 100 ) { 
			NSwimmers = n;
			Coords = new double[NSwimmers];
			Vels = new double[NSwimmers];
			for( int i = 0; i < NSwimmers; i++ ) Coords[i] = 0;
			for( int i = 0; i < NSwimmers; i++ ) {
				double v0 = 0.0001;
				Vels[i] = v0;
				v0 += ( v0 / 100 );
			}
			PoolLength = l;
		 } 
		 ~SwimmingPool() {
		 	delete[] Coords;
		 	delete[] Vels;
		 }
		 std::string ToString() {
		 	std::string Output = "Pool: " + std::to_string( PoolLength ) + " ";
		 	for( int i = 0; i < NSwimmers; i++ ) Output += std::to_string(Coords[i]) + " ";
		 	return Output;
		 }
		 
		 bool Check(int nswimmer) {
		 	return Coords[nswimmer] < PoolLength;
		 }
		 
		 bool CheckAll() {
		 	bool result = false;
		 	for( int i = 0; i < NSwimmers; i++ ) result = result || Check(i);
		 	return result;
		 }
		 bool Update( int swimmerNum ) {
		 	usleep(1);
		 	if( Check(swimmerNum) ) Coords[swimmerNum] += Vels[swimmerNum];
		 	if( Coords[swimmerNum] > PoolLength ) {
		 		Coords[swimmerNum] = PoolLength;
		 		return false;
		 	}
		 	else return true;
		 }
		 int GetNSwimmers() { return NSwimmers; }
};

//SwimmingPool *Pool =  new SwimmingPool;

void Swim( int id, SwimmingPool *pool) {
	while( pool->Update( id ) ); 
//		std::string Message = "Swimmer " + std::to_string(id) + " " + std::to_string(i) +"\n";
//		std::cout << Message;
//		usleep(swdelay);
	
}

bool Show( SwimmingPool *pool ) {
	bool result = pool->CheckAll();
	for( int i = 0; i <  pool->GetNSwimmers(); i++ ) std::cout << pool->ToString() + '\n';
	return result;
}

void Reporter ( SwimmingPool *pool, int reportdelay = 10 ) {
	while( Show( pool ) ) usleep(reportdelay);
}
void spawnThreads(SwimmingPool *pool ) {
	int n = pool->GetNSwimmers();
	std::vector<std::thread> threads( n  + 1 );
	for( int i = 0; i < n; i++) threads[i] = std::thread(Swim, i, pool );//, pool);
	threads[n] = std::thread(Reporter, pool, 10);
//	for(auto& th : threads) th.detach();
	for(auto& th : threads) th.join();  // works fine
 }




int main() {
	SwimmingPool *Pool =  new SwimmingPool;
	spawnThreads( Pool );
//	std::cout << Pool->ToString();
//	Show(pool);
	
//	int NSwimmers = 12;
//	int SwimDelay = 10;
//	int MaxSwim  = 5;
//	Swim(2, MaxSwim, SwimDelay);
//	spawnThreads( NSwimmers, MaxSwim, SwimDelay );
//	std::cout << "\nwait a moment...\n";
//	usleep(50000);
//	std::cout << "\nGO KILL THE POOL!\n";
	delete Pool;
//	usleep(3000);
//	std::cout << "DELeTED\n";
//	std::cout << "\second sleep\n";
//	usleep(50000);
//	std::cout << "waiting for return\n";
	return 0;
}

