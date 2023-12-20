#include <Windows.h>
#include <iostream>

#include "../Driver2/RLoggerCommon.h"

int main() {
    HANDLE hDevice = CreateFile(L"\\\\.\\RLogger", FILE_SHARE_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hDevice == INVALID_HANDLE_VALUE) {
        std::cout << "Error: driver must be running!\n";
        return 1;
    }

    std::string command;
    std::cout << "Enter command (setpid PID / stop): ";
    std::cin >> command;

    if (command == "setpid") {
        DWORD returned;
        ULONG pid;
        std::cout << "Enter PID: ";
        std::cin >> pid;

        if (DeviceIoControl(hDevice, IOCTL_RLOGGER_SET_TARGET_PID, &pid, sizeof(pid), nullptr, 0, &returned, nullptr)) {
            std::cout << "Success\n";
            return 0;
        }
        else {
            std::cout << "An error has occurred\n";
            return 1;
        }
    }

    if (command == "stop") {
        DWORD returned;

        if (DeviceIoControl(hDevice, IOCTL_RLOGGER_STOP_LOGGING, nullptr, 0, nullptr, 0, &returned, nullptr)) {
            std::cout << "Success\n";
            return 0;
        }
        else {
            std::cout << "An error has occurred\n";
            return 1;
        }
    }

    std::cout << "Error: Invalid command.\n";
    return 1;
}
