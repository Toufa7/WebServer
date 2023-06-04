#include <iostream>
#include <fstream>
#include <string>

using namespace std;
int main()
{
    std::ifstream   file;
    std::string     input;
    
    file.open("./text.txt");
    if  (file.is_open())
    {
        while (std::getline(file, input))
            std::cout << input << "\n";
    }
    else
        std::cerr << "Cannot Open\n";
}