#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

using namespace std;

class ThreadPool {
private:
    vector<thread> workers;

    queue<function<void()>> tasks;

    mutex queueMutex;

    condition_variable condition;

    bool stop;

    void worker();

public:
    ThreadPool(size_t threadCount);

    void enqueue(function<void()> task);

    ~ThreadPool();
};

#endif