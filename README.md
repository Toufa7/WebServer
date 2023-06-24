# Webserv

<p align="center">
<img src="./explanation/fds.jpg" width="600">


## DELETE Method Implementation :

<p align="center">
<img src="./explanation/delete.jpg" width="1000">


    int stat (const char *filename, struct stat *buf);
    
    struct stat {
        st_mode; // S_ISREG, S_ISDIR 
        {
            // File Permissions
            // {
                    // S_IRUSR : Read
                    // S_IWUSR : Write
                    // S_IXUSR : Execute
            // }
            
        }
        st_size; // File size in bytes
    }
    
https://www.mkssoftware.com/docs/man5/struct_stat.5.asp


    struct dirent *readdir(DIR *);
    struct dirent {
        d_type; // Type of file (DT_REG, DT_DIR)
        d_name[256]; // Filename
    }

https://stackoverflow.com/questions/12991334/members-of-dirent-structure

https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html


## GET Method Implementation :

## POST Method Implementation :


# Resources

https://www.gta.ufrj.br/ensino/eel878/sockets/index.html

https://stackoverflow.com/questions/6729366/what-is-the-difference-between-af-inet-and-pf-inet-in-socket-programming

https://stackoverflow.com/questions/76412255/how-to-fix-error-141-when-using-select-and-send-in-c-web-server-for-multip/76413445

https://stackoverflow.com/questions/5815675/what-is-sock-dgram-and-sock-stream



## TIPS:

if you want to look for a manuel of any function use this website mkssoftware/qnx (eg: mkssoftware select)
