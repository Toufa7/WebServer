NAME = Server
CFLAGS = c++ -std=c++98 -Wall -Wextra -Werror #-fsanitize=address -g

FILES = main.cpp \
	srcs/Server.cpp \
	srcs/Handler.cpp \
	srcs/Shared.cpp \
	srcs/Config.cpp \

HEADERS = includes/Server.hpp \
	includes/Client.hpp \
	includes/Shared.hpp \
	includes/Config.hpp \
	includes/Handler.hpp
	
OBJS = $(FILES:.cpp=.o)

all : $(NAME)

$(NAME): $(OBJS) $(HEADERS)
		$(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY : clean all fclean re