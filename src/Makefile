CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o utils.o symbol_table.o assemble_dt.o assemble_branch.o
emulate: emulate.o branch.o dp_reg.o dp_imm.o data_trans.o
test: test.o utils.o symbol_table.o assemble_dt.o

clean:
	$(RM) *.o assemble emulate
	
