NAME = Webserv
CC = c++
CFLAGS = -std=c++98 -Wall -Wextra -Werror
MKFL = Makefile

FILES = main.cpp \
	srcs/Server.cpp \
	srcs/Handler.cpp \
	srcs/Shared.cpp \
	srcs/Config.cpp \
	srcs/Cgi.cpp \

HEADERS = includes/Server.hpp \
	includes/Client.hpp \
	includes/Shared.hpp \
	includes/Config.hpp \
	includes/Handler.hpp \


all : $(NAME)

$(NAME): $(FILES) $(HEADERS) $(MKFL)
		$(CC) $(CFLAGS) $(FILES) -o $(NAME)

clean:
	rm -f $(NAME)
	rm -rf Server.dSYM

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY : clean all fclean re
