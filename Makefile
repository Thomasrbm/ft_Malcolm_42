NAME	= ft_malcolm

CC		= gcc
CFLAGS	= -Wall -Wextra -Werror

SRCS 	= main.c parse.c network.c arp.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re