/**
 * Original thread pool version: Copyright (c) 2012 Jakob Progsch, Václav Zeman
 */
#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <ctime>  
#include "badgerThread.h"
#include "stats.h"

namespace littleBadger {
    
  /**
   * ThreadPool is composed of lots of threads, and it manages those threads.
   */
  class ThreadManager {
  public:
    ThreadManager(size_t);
    
    void startThreadManager();
    void enqueueObj(BadgerThread);
    
    // template<class F, class... Args> 
    // auto enqueueFun(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    
    ~ThreadManager();
  private:
      // need to keep track of threads so we can join them
      std::vector<std::thread> workers;

      // the task queue: one for object, another for function
      std::queue<BadgerThread> tasksO; // use in this project, an object = a tack = a transaction
      // std::queue<std::function<void()>> tasks; // notuse in this project

      // synchronization
      std::condition_variable condition;
      std::mutex queue_mutex;
      std::mutex write_stat;
      bool start = false;
      bool stop;
      
      // processed tasks and start time
      int count = 0;
      std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
  };

  /**
   * the constructor launches maxThds of threads, each thread is constructed by a lambda closure.
   * ex. [capturing] {body} https://en.cppreference.com/w/cpp/language/lambda
   */
  inline ThreadManager::ThreadManager(size_t maxThds) : stop(false) {
    for (size_t i = 0; i < maxThds; ++i) {
      // emplace_back = construct an element in place using args as the arguments for its 
      // constructor and insert element at the end
      workers.emplace_back( 
        [this, i]{
          for (;;) {
            std::unique_lock<std::mutex> lock(this->queue_mutex); 
            // [this]{ return this->stop || !this->tasks.empty();} returns ture or false
            // when it return false, then condition.wait(lock, bool) will keep waiting.
            // when it return true, then we can move to next line if the thread receives a notify
            this->condition.wait(lock, 
                                [this]{return this->start && (this->stop || !this->tasksO.empty());});

            if (this->stop && this->tasksO.empty()) {
              std::cout << "thread end" << std::endl; 
              return;
            } 

            // get an olders task from queue and executes it
            // taskO = &(tasksO.front());
            BadgerThread newTask = tasksO.front();
            this->tasksO.pop();
            lock.unlock();

            // start time of this txn
            auto start = std::chrono::system_clock::now();
            newTask.run(i);

            // how long a txn takes
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double, std::ratio<1>> elapsed_seconds = end - start;
            newTask.duration = elapsed_seconds.count();
            
            // updata stats 
            this->write_stat.lock();
            setThroughput(newTask.duration);
            this->count++;
            this->write_stat.unlock();

            if (count % 1000 == 0) {
              std::cout << count << " txn end" << std::endl;
            }
          }
        }
      );
    }
  }

  /**
   * this method wakes threads up to handle queued tasks
   */
  void ThreadManager::startThreadManager() {
    start = true;
    condition.notify_all();
  }

  /**
   * enqueue new task to the queue
   */
  void ThreadManager::enqueueObj(BadgerThread bThread) {
      // don't allow enqueueing after stopping the pool
      if (stop) {
          throw std::runtime_error("enqueue on stopped ThreadPool");
      }

      tasksO.push(bThread);
  }

  /**
   * the destructor joins all threads
   */
  inline ThreadManager::~ThreadManager() {
      stop = true;
      condition.notify_all();

      int sizeThd = workers.size();
      for (int i = sizeThd - 1; i >= 0; i--) {
          workers[i].join();
      }

      std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
      std::chrono::duration<double> elapsed_seconds = endTime - startTime;
      std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

      getThroughput();
      getLatency();

      std::cout << "finish project" << std::endl; 
  }

  /** 
   * add new work item to the pool
   *
  template<class F, class... Args>
  auto ThreadPool::enqueueFun(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
      using return_type = typename std::result_of<F(Args...)>::type;

      auto task = std::make_shared<std::packaged_task<return_type()>>(
          std::bind(std::forward<F>(f), std::forward<Args>(args)...));
          
      std::future<return_type> res = task->get_future();
      std::unique_lock<std::mutex> lock(queue_mutex);

      // don't allow enqueueing after stopping the pool
      if (stop) {
          throw std::runtime_error("enqueue on stopped ThreadPool");
      }
          
      tasks.emplace([task](){ (*task)(); });
      
      condition.notify_one();
      return res;
  }
  */

  /**
   * std::condition_variable
   * 
   * The condition_variable class is a synchronization primitive that can be used to block a thread, 
   * or multiple threads at the same time, until another thread both modifies a shared variable (the 
   * condition), and notifies the condition_variable. 
   * The thread that intends to modify the variable has to 
   * 1. acquire a std::mutex (typically via std::lock_guard)
   * 2. perform the modification while the lock is held
   * 3. execute notify_one or notify_all on the std::condition_variable (the lock does not need to be 
   *    held for notification.
   */

  /** 
   * std::function<void()>
   * 
   * Class template std::function is a general-purpose polymorphic function wrapper. 
   * Instances of std::function can store, copy, and invoke any Callable target functions, lambda 
   * expressions, bind expressions, or other function objects, as well as pointers to member 
   * functions and pointers to data members.
   */

  /**
   * In C++11, there are two syntaxes for function declaration:
   * 1. return-type identifier ( argument-declarations... )
   * 2. auto identifier ( argument-declarations... ) -> return_type
   * 
   * https://stackoverflow.com/questions/22514855/arrow-operator-in-function-heading
   */

  /**
   * [capture] {body} is a lambda expression that builds an unnamed function
   * a lambda expression constructs a closure, an unnamed function object capable of capturing variables in scope
   * [this] = capture variable in this, {body} is the function body
   */
}

