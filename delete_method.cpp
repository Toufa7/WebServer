#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

void DeleteFile(const char *path)
{
    std::cout << path << ": File deleted successfully" << std::endl;
    unlink(path);
}

void    DeleteDirectory(const char *path)
{
    static int i = 0;
    i++;
    std::cout << i << std::endl;
    DIR             *directory;
    struct dirent   *dir;
    struct stat     file;
    char            subdir[256];

    // open a directory
    if ((directory = opendir(path)) == NULL)
    {
        std::cerr << "Cannot Open Directory: " << path << std::endl;
        return ;
    }
    // read the contents of the directory
    while ((dir = readdir(directory)) != NULL)
    {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            continue;
        snprintf(subdir, sizeof(subdir), "%s/%s", path, dir->d_name);
        if (stat(subdir, &file) == 0)
        {
            if (S_ISREG(file.st_mode))
            {
                std::cout << "File -> " << subdir << std::endl;
                DeleteFile(subdir);
            }
            else if (S_ISDIR(file.st_mode))
            {
                std::cout << "Dir -> " << subdir << std::endl;
                DeleteDirectory(subdir);
            }
        }
        else
        {
            std::cerr << "Error getting file or directory " << subdir << std::endl;
        }
    }
    closedir(directory);
    std::cout << "Path -> " << path << std::endl;
    if (rmdir(path) == 0)
    {
        std::cout << path << ": Directory deleted successfully" << std::endl;
    }
    else
    {
        std::cerr << "Error deleting directory " << path << std::endl;
    }
}

int main()
{
    const char  *path;
    struct stat file;

    path = "./Test/aa/";
    if (stat(path, &file) == 0)
    {
        if (S_ISREG(file.st_mode))
            DeleteFile(path);
        else if (S_ISDIR(file.st_mode))
            DeleteDirectory(path);
    }
    else
        std::cerr << "File or Directory doesn't exist :(" << std::endl;
}