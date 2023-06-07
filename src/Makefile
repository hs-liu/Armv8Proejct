CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall 

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o
emulate: emulate.o branch.o dp_reg_32.o dp_imm_32.o dp_reg_64.o dp_imm_64.o dt_32.o dt_64.o

clean:
	$(RM) *.o assemble emulate
	
