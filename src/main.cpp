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
    
    std::cout << input << ": command not found" << std::endl;
  }
  
  return 0;
}
