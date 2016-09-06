LIBS=-lzdk -lncurses -lm
DIRS=-I ./lib/ZDK -L ./lib/ZDK
FLAGS=-std=gnu99 -Wall -Werror -g

pong:
	gcc $(FLAGS) main.c -o bin/pong $(DIRS) $(LIBS)