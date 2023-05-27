#include <dirent.h>
#include <stdio.h>
#include <string>

int main (void)
{
    std::string str;
    DIR *d;
    struct dirent *dir;
    d = opendir ("/Users/ibnada/Desktop/webserv_test_dir");
    if (d)
    {
        while ((dir = readdir (d)) != NULL)
        {
            printf ("%s\n", dir->d_name);
        }
        closedir (d);
    }
    return (0);
}