// #include <iostream>
// #include <string>
// #include <vector>

// using namespace std;

// class Solution {
// public:
//     bool validPalindrome(string s)
//     {
//         int i,j;
//         std::cout << "Len -> " << s.length() << "\n";
//         if (s.length() % 2 == 0)
//         {
//             i = (s.length() / 2) - 1;
//             j = s.length() / 2;
//         }
//         else
//         {
//             i = s.length() / 2;
//             j = s.length() / 2;
//         }
//         while (i > 0 && j < s.length())
//         {
//             std::cout << s[i] << " VS " <<  s[j] << "\n";
//             if (s[i] == s[j])
//             {
//                 std::cout << s[i] << " == " <<  s[j] << "\n";
//                 i--;
//                 j++;
//             }
//             else
//             {
//                 s.erase(s.begin()+i);
//                 continue;
//                 std::cout << s[i] << " != " <<  s[j] << "\n";
//                 i--;
//                 j++;
//             }
//         }
//         return (0);
//     }
// };

// int main()
// {
//     Solution A;
//     std::cout << A.validPalindrome("abca");
// }


// // ebcbbececabbacecbbcbe