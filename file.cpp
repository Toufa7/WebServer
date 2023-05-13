#include <iostream>
#include <string>

int main() {
  int content_length = 42;
  std::string header_value;

  header_value.assign("Content-Length: " + std::to_string(content_length));

  std::cout << header_value << std::endl;  // Output: Content-Length: 42

  return 0;
}