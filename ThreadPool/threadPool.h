#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include<thread>
#include<mutex>
#include<condition_variable> 
#include<atomic>
#include<vector> 
#include<queue> 
#include<functional>

typedef std::function<void()> Task ; 

class ThreadPool {
private: 
    int thread_num ; // 线程总数
    std::atomic<bool> _is_running ; // 线程池是否关闭
    std::queue<Task> taskQueue ;  // 任务队列
    std::vector<std::thread> workThread ;  // 工作线程队列
    std::mutex conditional_mutex ; 
    std::condition_variable conditional_lock ; 

public :

    ThreadPool(const int number) : thread_num(number) , _is_running(false) {}

    ~ThreadPool() {

        if(_is_running) {
            stop() ; 
        }

    }

    void start() {
        _is_running = true ; 

        for(int i = 0 ; i < thread_num ; ++i){
            workThread.emplace_back(std::thread(&ThreadPool::work , this)) ; 
        }
    }

    void work() {
        printf("begin work thread: %d\n", std::this_thread::get_id());

        while(_is_running) {
            Task task ; 
            {
                std::unique_lock<std::mutex> locker(conditional_mutex) ; 
                if(!taskQueue.empty()) {

                    task = taskQueue.front() ; 
                    taskQueue.pop() ; 

                }else if(_is_running && taskQueue.empty() ){
                    conditional_lock.wait(locker)  ;
                }
            }

            if(task) {
                task() ; 
            }
        }
        printf("end work thread: %d\n", std::this_thread::get_id());
    }

    void appendTask(const Task &task) {
        if(_is_running) {
            // 确保互斥操作队列
            std::unique_lock<std::mutex> locker(conditional_mutex) ; 
            taskQueue.push(task) ; 
            conditional_lock.notify_one() ; // 唤醒一个线程去工作
        }
    }

    void stop() {

        _is_running = false ; 
        conditional_lock.notify_all() ; 
        for(auto &wt : workThread) {
            if(wt.joinable()) wt.join() ; 
        }

    }

} ; 

#endif // !_THREADPOOL_H_