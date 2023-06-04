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

        for (auto it : s)
        {
            if (it == '{' || it == '[' || it == '(')
                stk.push(it);
            else if (stk.empty() || ((it == ')') && (stk.top() != '(') )|| ((it == ']') && (stk.top() != '[')) || ((it == '}') && (stk.top() != '{')))
                return (false);
            else
                stk.pop();
        }
        return (stk.empty());
    }
};
