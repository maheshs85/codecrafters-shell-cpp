#include <iostream>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage

  std::string input;

  while (true) {
    std::cout << "$ ";
    std::getline(std::cin, input);

    // Check if the input is the exit command
    if (input == "exit 0") {
      break;
    }

    // If the command starts with "echo ", print the rest of the line
    if (input.compare(0, 5, "echo ") == 0) {
      std::cout << input.substr(5) << std::endl;
    }
    // If command is exactly "echo", print an empty line
    else if (input == "echo") {
      std::cout << std::endl;
    }
    // For any other input, display "command not found"
    else {
      std::cout << input << ": command not found" << std::endl;
    }
  }
  
  return 0;
}
