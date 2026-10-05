#include "../include/ThreadPool.h"
#include <iostream>

using namespace std;

ThreadPool::ThreadPool(size_t threadCount)
    : stop(false) {

    for (size_t i = 0; i < threadCount; ++i) {

        workers.emplace_back(
            [this]() {
                worker();
            }
        );
    }
}

void ThreadPool::worker() {

    while (true) {

        function<void()> task;

        {
            unique_lock<mutex> lock(queueMutex);

            condition.wait(
                lock,
                [this]() {
                    return stop || !tasks.empty();
                }
            );

            if (stop && tasks.empty()) {
                return;
            }

            task = move(tasks.front());

            tasks.pop();
        }

        task();
    }
}

void ThreadPool::enqueue(function<void()> task) {

    {
        lock_guard<mutex> lock(queueMutex);

        if (stop) {
            throw runtime_error(
                "Cannot enqueue task after ThreadPool has stopped."
            );
        }

        tasks.push(move(task));
    }

    condition.notify_one();
}

ThreadPool::~ThreadPool() {

    {
        lock_guard<mutex> lock(queueMutex);

        stop = true;
    }

    condition.notify_all();

    for (thread& workerThread : workers) {

        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
}