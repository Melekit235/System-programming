#include <iostream>
#include <locale>
#include <windows.h>
#include <fstream>
#include <thread>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include "ThreadPool.h"
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <string>

std::vector<std::string> data;
std::vector<std::vector<std::string>> sep_data;
std::string filename;
HANDLE hFile;
HANDLE hFileMapping;
LPVOID lpMapView;


bool GetDataFromFile()
{
    std::cout << "Enter filename - ";
    std::cin >> filename;
    std::cout << "\n";
    std::cout << "reading file..";
    std::cout << "\n";

    hFile = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error opening file.\n";
        return false;
    }

    hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);

    if (hFileMapping == NULL)
    {
        std::cerr << "Error creating file mapping.\n";
        CloseHandle(hFile);
        return false;
    }

    lpMapView = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (lpMapView == NULL)
    {
        std::cerr << "Error mapping view of file.\n";
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        return false;
    }

    char* fileData = static_cast<char*>(lpMapView);

    size_t fileSize = GetFileSize(hFile, NULL);

    std::string fileContent(fileData, fileSize);

    size_t pos = 0;
    while ((pos = fileContent.find('\n')) != std::string::npos)
    {
        std::string tmp_s = fileContent.substr(0, pos);
        data.push_back(tmp_s);
        fileContent.erase(0, pos + 1);
    }

    return true;
}

short GetThreadCapacity()
{
    short thread_quantity, max_thread = std::thread::hardware_concurrency();
    std::cout << "max number of thread - " << max_thread << std::endl;
    std::cout << "enter threads count - ";
    std::cin >> thread_quantity;
    std::cout << "\n";
    if ((thread_quantity > max_thread) || (thread_quantity <= 0))
        return -1;
    return thread_quantity;
}

void SortPiece(void* arr_piece)
{
    std::vector<std::string>* obj = (std::vector<std::string>*)arr_piece;
    std::sort(obj->begin(), obj->end());
}

int GetMinValueIndex(std::vector<std::string> indata)
{
    std::string min_value;
    int index;
    for (int i = 0; i < indata.size(); i++)
        if (indata[i] != "")
        {
            min_value = indata[i];
            index = i;
            break;
        }

    for (int i = 0; i < indata.size(); i++)
        if (indata[i] != "")
            if (strcmp(min_value.c_str(), indata[i].c_str()) > 0)
                if (min_value > indata[i])
                {
                    min_value = indata[i];
                    index = i;
                }
    return index;
}

bool CorrectBuff(std::vector<std::string> indata)
{
    for (int i = 0; i < indata.size(); i++)
        if (indata[i] != "")
            return true;
    return false;
}

void FinalSort()
{
    int result_size = data.size();
    data.clear();
    std::vector<int> counter;
    counter.reserve(sep_data.size());
    for (int i = 0; i < sep_data.size(); i++)
        counter.push_back(0);
    std::vector<std::string> buff;

    while (1)
    {
        buff.clear();
        for (int i = 0; i < sep_data.size(); i++)
            if (sep_data[i].size() > counter[i])
                buff.push_back(sep_data[i][counter[i]]);
            else
                buff.push_back("");
        if (!CorrectBuff(buff))
            break;
        int minvalue_index = GetMinValueIndex(buff);
        data.push_back(buff[minvalue_index]);
        counter[minvalue_index]++;
    }
}

bool WriteDataToFile()
{
    char* fileData = static_cast<char*>(lpMapView);


    for (const auto& str : data)
    {
        const char* cstr = str.c_str();
        size_t length = strlen(cstr);
        CopyMemory(fileData, cstr, length);
        fileData += length;
        *fileData = '\n'; 
        ++fileData;
    }

    UnmapViewOfFile(lpMapView);
    CloseHandle(lpMapView);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);

    std::cout << "Data written to file: " << filename << std::endl;

    return true;
}

void CreateTasks(std::vector<std::string> indata, short quantity, TaskQueue* result)
{
    bool isEven = indata.size() % quantity == 0;
    int standard_string_capacity = indata.size() / quantity;
    if (!isEven)
        standard_string_capacity = indata.size() / quantity + 1;
    int global_counter = 0;
    for (int i = 0; i < quantity; i++)
    {
        sep_data[i].clear();
        int tmp_counter = 0;
        while (standard_string_capacity != tmp_counter)
        {
            if (global_counter == indata.size())
                break;
            sep_data[i].push_back(indata[global_counter]);
            global_counter++;
            tmp_counter++;
        }
        Task* newtask = new Task(&SortPiece, (void*)&sep_data[i]);
        result->Enqueue(newtask);
    }
}

int main()
{
    if (!GetDataFromFile())
    {
        std::cout << "file not founded." << std::endl;
        system("pause");
        return 1;
    }

    short thread_quantity = GetThreadCapacity();
    if (thread_quantity == -1)
    {
        std::cout << "invalid thread count value." << std::endl;
        system("pause");
        return 1;
    }
    sep_data.resize(thread_quantity);

    TaskQueue* taskqueue = new TaskQueue();
    CreateTasks(data, thread_quantity, taskqueue);
    ThreadPool* threads = new ThreadPool(thread_quantity, taskqueue);
    threads->WaitAll();
    delete threads;
    delete taskqueue;

    FinalSort();

    std::cout << "DONE\n" << std::endl;

    if (WriteDataToFile())
    {
        std::cout << "Done" << std::endl;
    }

    data.clear();
    system("pause");
    return 0;
}
