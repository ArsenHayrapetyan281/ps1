#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>
#include <vector>
#include <cstring>

void do_command(char** argv) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    pid_t pid = fork();
    
    if (pid == -1) {
        std::cerr << "Fork failed!" << std::endl;
        return;
    }

    if (pid == 0) {
        if (execvp(argv[0], argv) == -1) {
            std::cerr << "Exec failed: " << strerror(errno) << std::endl;
            exit(1);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end_time - start_time;
        
        if (WIFEXITED(status)) {
            std::cout << "Command completed with exit code " << WEXITSTATUS(status) 
                      << " and took " << duration.count() << " seconds." << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cout << "Command terminated by signal " << WTERMSIG(status) 
                      << " and took " << duration.count() << " seconds." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./do-command <command> [arguments]" << std::endl;
        return 1;
    }

    std::vector<char*> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    args.push_back(nullptr);

    do_command(args.data());

    return 0;
}

