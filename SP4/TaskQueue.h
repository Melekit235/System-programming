#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <windows.h>

#include "Task.h"

class Node
{
public:
    Task* data;
    Node* prev;
    Node* next;
};

class TaskQueue
{
private:
    Node* first, * last;
    HANDLE mutex;

public:
    TaskQueue();
    ~TaskQueue();

    Task* Front();
    void Enqueue(Task* indata);
    void Dequeue();
    bool Empty();
};

#endif
