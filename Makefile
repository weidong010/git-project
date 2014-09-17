SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c = .o)
CC = gcc
LDLIB = -lpthread -lgomp
CCFLAGS = -g -Wall -O0
Test_Main : $(OBJS)
	    $(CC) $^ -o $@ $(LDLIB)
%.o : %.c
	    $(CC) -c $< $(CCFLAGS)
	
clean:    
	    rm *.o Test_Main 
.PHONY:clean