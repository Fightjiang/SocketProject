#include <iostream> 
#include <thread> 
#include <condition_variable> 
#include <mutex>

using namespace std ; 

int count = 0 ; 
mutex mu ; 
condition_variable condition ; 


void print_odd(){
	while(count < 100 ) {
		std::unique_lock<mutex> locker(mu) ; 
		condition.wait(locker , []{return count % 2 == 1 ; }) ;
		cout<<"print from odd : count = " << count++ <<endl ; 
		condition.notify_one() ; 
	}
}

void print_even() {
	while(count < 100) {
		std::unique_lock<mutex> locker(mu) ; 
		condition.wait(locker , []{return count % 2 == 0 ; }) ;
		cout<<"print from even : count = " << count++ <<endl ; 
		condition.notify_one() ; 
	}
}

int main() {
	thread th1 = thread(print_odd) ; 
	thread th2 = thread(print_even) ; 
	th1.join() ;
	th2.join() ; 

	return 0 ; 
}