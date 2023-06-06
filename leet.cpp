#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    void    groupAnagrams(vector<string>& strs)
    {
        vector<vector<string>> dupstr(5);
        vector<string> sortstrs = strs;
    
        for (int i = 0; i < strs.size(); i++)
            std::sort(sortstrs[i].begin(),sortstrs[i].end());
        
        for (int i = 0; i < sortstrs.size(); i++)
        {
            vector<string> tmp;
            for (int j = 0; j < sortstrs.size(); j++)
            {
                if (sortstrs[i] == sortstrs[j])
                {
                    tmp.push_back(sortstrs[j]);

                }
                    // dupstr[0].push_back(strs[j]);
            }
        }
        for (int i = 0; i < dupstr[0].size(); i++)
        {
            std::cout << dupstr[0][i] << "\n";

        }
        // return (dupstr);        
    }
};

int main()
{
    Solution A;
    vector<string> strs {"eat","tea","tan","ate","nat","bat"};
    A.groupAnagrams(strs);
}


// // ebcbbececabbacecbbcbe