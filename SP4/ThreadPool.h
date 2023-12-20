#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "TaskQueue.h"

class ThreadPool
{
public:
    ThreadPool(int thread_quantity, TaskQueue* task_queue);
    void WaitAll();
    ~ThreadPool();

private:
    HANDLE* threads;
    int thread_quantity;
    static DWORD WINAPI DoThreadWork(LPVOID obj);
};

#endif
