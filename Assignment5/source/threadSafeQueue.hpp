#ifndef THREADSAFEQUEUE
#define THREADSAFEQUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class ThreadSafeQueue
{
public:
  ThreadSafeQueue(void): q(), m(), c(){}

  void enqueue(T t){
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    c.notify_one();
  }

  T dequeue(void){
    std::unique_lock<std::mutex> lock(m);
    while(q.empty())
    {
      c.wait(lock);
    }
    T val = q.front();
    q.pop();
    return val;
  }

  int getSize() {
    std::unique_lock<std::mutex> lock(m);
    return q.size();
  }

  ~ThreadSafeQueue(void){}

private:
  std::queue<T> q;
  mutable std::mutex m;
  std::condition_variable c;
};
#endif
