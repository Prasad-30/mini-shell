OBJ:= $(patsubst %.c, %.o, $(wildcard *.c))
minishell.exe : $(OBJ)
	gcc -o $@ $^
clean:
	rm *.o *.exe