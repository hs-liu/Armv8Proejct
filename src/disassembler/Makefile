CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall

.SUFFIXES: .c .o

.PHONY: all clean

all: disassemble

disassemble: disassemble.o dp_reg.o dp_imm.o data_trans.o branch.o

clean:
	$(RM) *.o disassemble
	
