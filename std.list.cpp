#include <iostream>
#include  <list>



int main()
{
    std::list<int> lst;

    for (int i = 0; i < 10; i++)
    {
        lst.push_back(i);
        if (i == 5)
        for (; i < 10; i++)
            lst.push_front(i);
    }
    std::list<int>::iterator it = lst.begin();
    for (; it != lst.end();it++)
        std::cout << *it << "\n";

    std::cout << "----------\n";
    
    while (*it != 0)
        it++;
    lst.erase(it);
    for (it = lst.begin(); it != lst.end();it++)
        std::cout << *it << "\n";
    // for (it = lst.begin() ; it != lst.end();it++)
    //     std::cout << *it << "\n";
} 
