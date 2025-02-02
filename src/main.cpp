#include <iostream>
#include <string>

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
                    std::cout << arg << ": not found" << std::endl;
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
