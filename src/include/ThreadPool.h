#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
  typedef std::function<void()> Task;

  ThreadPool(int threadnum = 0);
  ~ThreadPool();

  void Start();
  void Stop();
  void AddTask(Task task);
  void ThreadFunc();
  int GetThreadNum() { return this->threadnum_; }

private:
  bool started_;
  int threadnum_;
  std::vector<std::thread*> threadlist_;
  std::queue<Task> taskqueue_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

#endif