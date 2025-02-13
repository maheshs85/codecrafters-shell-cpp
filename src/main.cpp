#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <sys/wait.h>
#include <cstring>

enum class Command {
    ECHO,
    EXIT,
    TYPE,
    PWD,
    CD,
    UNKNOWN
};

const std::unordered_map<std::string, Command> COMMAND_MAP = {
    {"echo", Command::ECHO},
    {"exit", Command::EXIT},
    {"type", Command::TYPE},
    {"pwd", Command::PWD},
    {"cd", Command::CD}
};


std::string get_command_name(const std::string& input) {
    std::stringstream ss(input);
    std::string cmd;
    ss >> cmd;
    return cmd;
}

Command identify_command(const std::string& input) {    
    std::string cmd = get_command_name(input);
    auto it = COMMAND_MAP.find(cmd);
    return (it != COMMAND_MAP.end()) ? it->second : Command::UNKNOWN;
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

std::string expand_tilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }
    
    const char* home = std::getenv("HOME");
    if (!home) {
        return path;  // If HOME is not set, return original path
    }
    
    // Replace ~ with home directory
    if (path.length() == 1) {  // Just "~"
        return home;
    }
    // "~/something" -> "/home/user/something"
    return std::string(home) + path.substr(1);
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

std::string get_current_directory() {
    return std::filesystem::current_path().string();
}

void execute_custom_executable(const std::string& input) {
    std::vector<std::string> args;
    std::stringstream ss(input);
    std::string arg;

    while (ss >> arg) {
        args.push_back(arg);
    }

    if (args.empty()) return;

    std::string exe_path = find_executable(args[0]);
    if (exe_path.empty()) {
        std::cout << args[0] << ": command not found" << std::endl;
        return;
    }

    int result = std::system(input.c_str());
    if (result == -1) {
        std::cerr << "Failed to execute " << exe_path << std::endl;
    }
}

bool is_builtin(const std::string& cmd) {
    return COMMAND_MAP.find(cmd) != COMMAND_MAP.end();
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
                if (is_builtin(arg)) {
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
        case Command::PWD:
            std::cout << get_current_directory() << std::endl;
            break;
        case Command::CD: {
            std::string dir = input.substr(3);
            if (dir.empty()) {
                std::cerr << "cd: missing argument" << std::endl;
            } else {
                std::string expanded_dir = expand_tilde(dir);
                if (chdir(expanded_dir.c_str()) != 0) {
                    std::cerr << "cd: " << dir << ": " << strerror(errno) << std::endl;
                }
            }
            break;
        }
        case Command::UNKNOWN:
            execute_custom_executable(input);
            break;
        case Command::EXIT:
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
        if (cmd == Command::EXIT) {
            break;  // Exit the shell
        }
        execute_command(input, cmd);
    }
    return 0;
}
