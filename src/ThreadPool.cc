#include <deque>
#include <unistd.h>
#include "ThreadPool.h"

ThreadPool::ThreadPool(int threadnum)
    : started_(false), threadnum_(threadnum), threadlist_(),
      taskqueue_(), mutex_(), cv_() {}

ThreadPool::~ThreadPool() {
  this->Stop();
  for (int i = 0; i < this->threadnum_; i++) {
    this->threadlist_[i]->join();
    delete this->threadlist_[i];
  }
  this->threadlist_.clear();
}

void ThreadPool::Start() {
  if (this->threadnum_ > 0) {
    this->started_ = true;
    for (int i = 0; i < this->threadnum_; ++i) {
      std::thread* th = new std::thread(&ThreadPool::ThreadFunc, this);
      this->threadlist_.push_back(th);
    }
  }
}

void ThreadPool::Stop() {
  this->started_ = false;
  this->cv_.notify_all();
}

void ThreadPool::AddTask(Task task) {
  std::lock_guard<std::mutex> lock(mutex_);
  this->taskqueue_.push(task);
  this->cv_.notify_one();
}

void ThreadPool::ThreadFunc() {
  std::thread::id tid = std::this_thread::get_id();
  std::cout << "worker thread is starting :" << tid << std::endl;
  Task task;
  while (this->started_) {
    task = NULL;
    {
      std::unique_lock<std::mutex> lock(this->mutex_);
      while (this->taskqueue_.empty() && this->started_) {
        this->cv_.wait(lock);
      }
      if (!this->started_)
        return;
      task = this->taskqueue_.front();
      this->taskqueue_.pop();
    }
    if (task) {
      task();
    }
  }
}