#include <iostream> 
#include <thread> 
#include <condition_variable> 
#include <mutex>
#include <queue>

using namespace std ; 

queue<int> q ; 
mutex mutex1 , mutex2 ; 
condition_variable produce , consume ; 
int maxSize = 20 ; 
// 生产者与消费者存在同步关系，而且各个生产者之间、各个消费者之间存在互斥关系,他们必须互斥地访问缓冲区。
void producer() {
	while(true) {

		std::unique_lock<mutex> locker(mutex1) ; 
		produce.wait(locker , []{return q.size() != maxSize ; }) ; 

		//mutex2.lock() ; // 互斥使用队列
		int tmpNumber = rand() % 100 ; 
		cout << "-> " << this_thread::get_id() << " produce number : " << tmpNumber <<" size = " << q.size() << endl ;
		q.push(tmpNumber) ; 
		//mutex2.unlock() ; 

		consume.notify_all() ; 
	}
}

void consumer() {
	while(true) {
		std::unique_lock<mutex> locker(mutex1) ; 
		consume.wait(locker , []{return q.empty() == false ; }) ; 

		//mutex2.lock() ; // 互斥使用队列
		cout << "-> " << this_thread::get_id() << " consume number : " << q.front() <<" size = " << q.size() << endl ;
		q.pop() ;
		//mutex2.unlock() ; 

		produce.notify_all() ;
	}
}


int main() {

	thread consumers[2], producers[2];

    // spawn 2 consumers and 2 producers:
    for (int i = 0; i < 2; ++i)
    {
        consumers[i] = thread(consumer);
        producers[i] = thread(producer);
    }

    // join them back: (in this program, never join...)
    for (int i = 0; i < 2; ++i)
    {
        producers[i].join();
        consumers[i].join();
    }

	return 0 ; 
}