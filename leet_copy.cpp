#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

class Solution {
public:
    bool isValid(string s)
    {
        std::stack<char> stk;
        stk.push(s[0]);
        int i = 1;
        while (i < s.length())
        {
            if (stk.top() == '{')
            {
                if (s[i] == '}')
                {
                    stk.pop();
                    if (i + 1 == s.length())
                        return (0);
                }
                else
                    stk.push(s[i]);
            }
            if (stk.top() == '[')
            {
                if (s[i] == ']')
                {
                    stk.pop();
                    if (i + 1 == s.length())
                        return (0);
                }
                else
                    stk.push(s[i]);
            }
            if (stk.top() == '(')
            {
                if (s[i] == ')')
                {
                    stk.pop();
                    if (i + 1 == s.length())
                        return (0);
                    // continue;
                }
                else
                    stk.push(s[i]);
            }
            std::cout << stk.size() << " <- Size\n";

            i++;
        }
        return (stk.empty() ? 0 : 1);
    }
};

int main()
{
    Solution A;
    std::cout << A.isValid("([])");
}


// ebcbbececabbacecbbcbe