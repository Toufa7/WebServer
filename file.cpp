#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

int main(int ac, char **av)
{
    int fd = open("test/poms.jpeg", O_RDONLY);
    struct stat a;
    fstat(fd, &a);
    std::cout << a.st_size;

    return 0;
}
