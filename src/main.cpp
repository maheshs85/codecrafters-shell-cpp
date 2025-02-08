#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <sstream>
#include <unistd.h>

enum class Command {
    ECHO,
    EXIT,
    TYPE,
    UNKNOWN
};

Command identify_command(const std::string& input) {
    if (input == "exit 0") return Command::EXIT;
    if (input == "echo" || input.compare(0, 5, "echo ") == 0) return Command::ECHO;
    if (input == "type" || input.compare(0, 5, "type ") == 0) return Command::TYPE;
    return Command::UNKNOWN;
}

bool is_executable(const std::filesystem::path& path) {
    return std::filesystem::exists(path) && access(path.c_str(), X_OK) == 0;
}

std::vector<std::string> split_path(const std::string& path_var) {
    std::vector<std::string> paths;
    std::stringstream ss(path_var);
    std::string path;

    while (std::getline(ss, path, ':')) {
        if (!path.empty()) {
            paths.push_back(path);
        }
    }

    return paths;
}

std::string find_executable(const std::string& cmd) {
    const char* path_var = std::getenv("PATH");
    if (!path_var) return "";

    for (const auto& dir : split_path(path_var)) {
        std::filesystem::path full_path = std::filesystem::path(dir) / cmd;
        if (is_executable(full_path)) {
            return full_path.string();
        }
    }
    return "";
}

void execute_command(const std::string& input, Command cmd) {
    switch (cmd) {
        case Command::ECHO:
            if (input == "echo") {
                std::cout << std::endl;
            } else {
                std::cout << input.substr(5) << std::endl;
            }
            break;
        case Command::TYPE:
            if (input == "type") {
                std::cout << "type: missing argument" << std::endl;
            } else {
                std::string arg = input.substr(5);
                if (arg == "echo" || arg == "exit" || arg == "type") {
                    std::cout << arg << " is a shell builtin" << std::endl;
                } else {
                    std::string exe_path = find_executable(arg);
                    if (!exe_path.empty()) {
                        std::cout << arg << " is " << exe_path << std::endl;
                    } else {
                        std::cout << arg << ": not found" << std::endl;
                    }
                }
            }
            break;
        case Command::UNKNOWN:
            std::cout << input << ": command not found" << std::endl;
            break;
    }
}

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::string input;


    while (true) {
        std::cout << "$ ";
        std::getline(std::cin, input);
        
        Command cmd = identify_command(input);
        if (cmd == Command::EXIT) break;
        execute_command(input, cmd);
    }
    return 0;
}
