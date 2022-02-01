#include "../../list.hpp"

#include <string>
#include <iostream>

void StartMiniChat();

int main() {
  StartMiniChat();
}

void StartMiniChat() {
  List<std::string> history;
  std::cout << "Hello, I am a small bot! Send me messages and I will respond. If you want to stop the conversation print \"exit\"." << std::endl;
  std::string last_msg;

  while (last_msg != "exit") {
    std::cin >> last_msg;
    history.push_back(last_msg);
    std::cout << "Wow!" << std::endl;
  }

  std::cout << "\nThank you for the chat! See you next time, and here is the full our conversation.\n" << std::endl;
  history.Print();
}
