#include "TaskQueue.h"
#include "iostream"

using namespace std;

TaskQueue::TaskQueue()
{
    mutex = CreateMutex(NULL, FALSE, NULL);
    first = NULL;
    last = NULL;
}

Task* TaskQueue::Front()
{
    WaitForSingleObject(mutex, INFINITE);

    if (Empty())
    {
        ReleaseMutex(mutex);
        return NULL;
    }
    if (first == last)
    {
        Node* tmp_return = first;
        first = NULL;
        last = NULL;
        ReleaseMutex(mutex);
        Task* result = tmp_return->data;
        delete tmp_return;
        return result;
    }

    Node* tmp_last = last;
    last = last->prev;
    last->next = NULL;
    ReleaseMutex(mutex);
    Task* result = tmp_last->data;
    delete tmp_last;
    return result;
}

void TaskQueue::Enqueue(Task* indata)
{
    WaitForSingleObject(mutex, INFINITE);

    Node* tmp = new Node();
    tmp->data = indata;
    tmp->next = NULL;
    tmp->prev = NULL;

    if (Empty())
    {
        first = tmp;
        last = tmp;
    }
    else
    {
        Node* tmp_first = first;
        first = tmp;
        first->next = tmp_first;
        tmp_first->prev = first;
    }

    ReleaseMutex(mutex);
}

void TaskQueue::Dequeue()
{
    Task* tmp = Front();
    if (tmp != nullptr)
    {
        delete tmp;
    }
}

bool TaskQueue::Empty()
{
    return ((first == NULL) && (last == NULL));
}

TaskQueue::~TaskQueue()
{
    while (!Empty())
        Dequeue();

    CloseHandle(mutex);
}
