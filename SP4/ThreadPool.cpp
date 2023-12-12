#include "ThreadPool.h"
#include "iostream"
#include <vector>

using namespace std;

ThreadPool::ThreadPool(int thread_quantity, TaskQueue* task_queue)
{
    threads = (HANDLE*)malloc(sizeof(HANDLE) * thread_quantity);
    this->thread_quantity = thread_quantity;

    for (int i = 0; i < thread_quantity; i++)
    {
        threads[i] = CreateThread(NULL, 0, &ThreadPool::DoThreadWork, (LPVOID)task_queue, CREATE_SUSPENDED, NULL);
        ResumeThread(threads[i]);
    }
}

void ThreadPool::WaitAll()
{
    WaitForMultipleObjects(thread_quantity, threads, TRUE, INFINITE);
}

ThreadPool::~ThreadPool()
{

    for (int i = 0; i < thread_quantity; i++)
    {
        CloseHandle(threads[i]);
    }

    free(threads);
}

DWORD WINAPI ThreadPool::DoThreadWork(LPVOID obj)
{
    TaskQueue* tasks = (TaskQueue*)obj;

    while (1)
    {
        Task* tmp = tasks->Front();

        if (tmp == NULL)
        {
            ExitThread(0);
        }
        std::vector<std::string>* dataPiece = (std::vector<std::string>*)tmp->params;
        std::cout << "     Thread ID: " << GetCurrentThreadId() << ", Data: ";
        for (const std::string& str : *dataPiece) {
            std::cout << str << " ";
        }
        std::cout << std::endl;

        tmp->func(tmp->params);
        dataPiece = (std::vector<std::string>*)tmp->params;
        std::cout << "     Thread ID: " << GetCurrentThreadId() << ", Data after sorting: ";
        for (const std::string& str : *dataPiece) {
            std::cout << str << " ";
        }
        std::cout << std::endl;
        delete tmp;
    }

    return 0;
}
