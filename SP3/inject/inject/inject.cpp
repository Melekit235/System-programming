#include <iostream>
#include <windows.h>
#include <conio.h>
//#include <stdio.h>


int main()
{
    //const char str[] = { 'w', 'h', 'y', ' ', 'a', 'm', ' ', 'i', ' ', 'd', 'o', 'i', 'n', 'g', ' ', 't', 'h', 'i', 's', '\0'};
    const char str[] = { 'w', 'h', 'y', '\0' };

    while (true)
    {
        std::cout << str << std::endl;
        _getch();
    }
}