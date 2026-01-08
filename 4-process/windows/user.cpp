#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <cstdlib>
#include <vector>
#include <cstring>

// Конвертация wstring в string
std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), 
                                          NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), 
                       &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

bool isProcessRunning(DWORD pid) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "[User] : failed to create process snapshot!" << std::endl;
        return false;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    bool found = false;
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (pe32.th32ProcessID == pid) {
                found = true;
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return found;
}

DWORD spawnTestProcess(const std::string& processName) {
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION pi = { 0 };
    
    std::wstring wProcessName;
    std::wstring commandLine;
    
    // Для разных тестовых процессов
    if (processName == "notepad") {
        wProcessName = L"C:\\Windows\\System32\\notepad.exe";
        commandLine = wProcessName;
    } else if (processName == "calc") {
        wProcessName = L"C:\\Windows\\System32\\calc.exe";
        commandLine = wProcessName;
    } else if (processName == "sleep") {
        // Альтернатива для теста - создаем собственный "sleep" процесс
        wProcessName = L"C:\\Windows\\System32\\cmd.exe";
        commandLine = L"cmd.exe /c timeout 100 > nul";
    } else {
        std::cerr << "[User] : unknown process name: " << processName << std::endl;
        return 0;
    }
    
    std::vector<wchar_t> cmdLine(commandLine.begin(), commandLine.end());
    cmdLine.push_back(L'\0');
    
    if (CreateProcessW(
        wProcessName.empty() ? NULL : wProcessName.c_str(),
        cmdLine.data(),
        NULL, NULL, FALSE,
        CREATE_NO_WINDOW, // Скрываем окно
        NULL, NULL,
        &si, &pi)) {
        
        std::cout << "[User] : spawned " << processName << " with pid " << pi.dwProcessId << std::endl;
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        
        // Даем процессу время запуститься
        Sleep(1000);
        
        return pi.dwProcessId;
    } else {
        DWORD error = GetLastError();
        std::cerr << "[User] : failed to spawn " << processName 
                  << ". Error: " << error << std::endl;
        return 0;
    }
}

void killProcess(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess) {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
}

int main() {
    std::cout << "=== User Application for testing Killer ===" << std::endl;
    
    // Проверяем существование killer.exe
    if (GetFileAttributesA("killer.exe") == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Error: 'killer.exe' not found!" << std::endl;
        std::cerr << "Please compile killer.cpp first: g++ -o killer.exe killer.cpp" << std::endl;
        return 1;
    }

    std::cout << "\n>>> TEST 1: Testing flag --id" << std::endl;
    DWORD pid1 = spawnTestProcess("notepad");
    
    if (pid1 > 0) {
        std::cout << "[User] : created process with PID: " << pid1 << std::endl;
        
        std::string cmd1 = "killer.exe --id " + std::to_string(pid1);
        std::cout << "[User] : executing: " << cmd1 << std::endl;
        
        system(cmd1.c_str());
        
        Sleep(2000); // Даем время на завершение
        
        if (!isProcessRunning(pid1)) {
            std::cout << "[User]   SUCCESS: Process " << pid1 << " is dead." << std::endl;
        } else {
            std::cerr << "[User]   FAILURE: Process " << pid1 << " is still alive!" << std::endl;
            killProcess(pid1);
        }
    }

    std::cout << "\n>>> TEST 2: Testing flag --name" << std::endl;
    DWORD pid2 = spawnTestProcess("calc");
    
    if (pid2 > 0) {
        std::cout << "[User] : created process with PID: " << pid2 << std::endl;
        Sleep(1000); // Даем калькулятору время запуститься
        
        std::cout << "[User] : executing: killer.exe --name calc.exe" << std::endl;
        system("killer.exe --name calc.exe");
        
        Sleep(2000);
        
        if (!isProcessRunning(pid2)) {
            std::cout << "[User]   SUCCESS: Process " << pid2 << " is dead." << std::endl;
        } else {
            std::cerr << "[User]   FAILURE: Process " << pid2 << " is still alive!" << std::endl;
            killProcess(pid2);
        }
    }

    std::cout << "\n>>> TEST 3: Testing environment variable PROC_TO_KILL" << std::endl;
    DWORD pid3 = spawnTestProcess("notepad");
    
    if (pid3 > 0) {
        std::cout << "[User] : created process with PID: " << pid3 << std::endl;
        
        // Устанавливаем переменную окружения (для MinGW используем putenv)
        std::string envVar = "PROC_TO_KILL=notepad.exe,calc.exe";
        putenv(envVar.c_str());
        std::cout << "[User]   Environment variable PROC_TO_KILL set to: notepad.exe,calc.exe" << std::endl;
        
        std::cout << "[User] : executing: killer.exe" << std::endl;
        system("killer.exe");
        Sleep(2000);
        
        if (!isProcessRunning(pid3)) {
            std::cout << "[User]   SUCCESS: Process " << pid3 << " killed via Env Var." << std::endl;
        } else {
            std::cerr << "[User]   FAILURE: Process " << pid3 << " is still alive!" << std::endl;
            killProcess(pid3);
        }
        
        // Удаляем переменную окружения
        putenv("PROC_TO_KILL=");
    }

    std::cout << "\n=== All tests completed ===" << std::endl;
    
    // Финальная проверка - убедимся, что нет случайно оставшихся процессов
    std::cout << "\n[User] : cleaning up any remaining test processes..." << std::endl;
    system("killer.exe --name notepad.exe");
    system("killer.exe --name calc.exe");
    system("killer.exe --name cmd.exe");
    
    Sleep(1000);
    
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    
    return 0;
}