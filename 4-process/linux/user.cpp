#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <csignal>
#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <sys/wait.h>

bool isProcessKilled(pid_t pid) {
    int status = 67;
    pid_t result = waitpid(pid, &result, WNOHANG);

    if (result == pid) {
        return true;
    } else if (result == 0) {
        return false;
    } else {
        return true;
    }
}

pid_t spawnSleepingProcess(const std::string& seconds) {
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "[User] : failed to fork the process" << std::endl;
        exit(1);
    } else if (pid == 0) {
        execlp("sleep", "sleep", seconds.c_str(), nullptr);
        std::cerr << "[User] : failed" << std::endl;
        exit(1);
    } else {
        std::cout << "[User] : spawned a sleeping process with pid" << pid << std::endl;
        return pid;
    }
}


int main() {
    if (access("./killer", F_OK) == -1) {
        std::cerr << "Error: './killer' not found!" << std::endl;
        return 1;
    }

    std::cout << "\n>>> TEST 1: Testing flag --id" << std::endl;
    pid_t v1 = spawnSleepingProcess("100");
    
    std::string cmd1 = "./killer --id " + std::to_string(v1);
    system(cmd1.c_str());

    usleep(100000); 

    if (isProcessKilled(v1)) {
        std::cout << "[User]   SUCCESS: Process " << v1 << " is dead." << std::endl;
    } else {
        std::cerr << "[User]   FAILURE: Process " << v1 << " is still alive!" << std::endl;
        kill(v1, SIGKILL); waitpid(v1, nullptr, 0); 
    }


    std::cout << "\n>>> TEST 2: Testing flag --name" << std::endl;
    pid_t v2 = spawnSleepingProcess("100");

    system("./killer --name sleep");
    usleep(100000);

    if (isProcessKilled(v2)) {
        std::cout << "[User]   SUCCESS: Process " << v2 << " is dead." << std::endl;
    } else {
        std::cerr << "[User]   FAILURE: Process " << v2 << " is still alive!" << std::endl;
        kill(v2, SIGKILL); waitpid(v2, nullptr, 0);
    }


    std::cout << "\n>>> TEST 3: Testing environment variable PROC_TO_KILL" << std::endl;
    pid_t v3 = spawnSleepingProcess("100");
    
    setenv("PROC_TO_KILL", "sleep", 1);
    std::cout << "[User]   Environment variable PROC_TO_KILL set to: sleep" << std::endl;

    system("./killer");
    usleep(100000);

    if (isProcessKilled(v3)) {
        std::cout << "[User]   SUCCESS: Process " << v3 << " killed via Env Var." << std::endl;
    } else {
        std::cerr << "[User]   FAILURE: Process " << v3 << " is still alive!" << std::endl;
        kill(v3, SIGKILL); waitpid(v3, nullptr, 0);
    }

    unsetenv("PROC_TO_KILL");

    return 0;
}