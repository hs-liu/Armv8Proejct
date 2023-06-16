CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate

emulate: emulate.o branch.o dp_reg.o dp_imm.o data_trans.o

clean:
	$(RM) *.o emulate
	
