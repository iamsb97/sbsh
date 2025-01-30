sbsh: main.c builtin.c helper.c
	gcc -g -Wall -Werror -pedantic-errors -o sbsh main.c builtin.c helper.c

