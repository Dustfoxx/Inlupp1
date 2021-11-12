# VARIABLES
C_COMPILER     = gcc
C_OPTIONS      = -Wall -pedantic -g -pg
C_LINK_OPTIONS = -lm 
CUNIT_LINK     = -lcunit


freq: list.o hash.o freq.o
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) hash_table.o linked_list.o freq-count.o -o freq

hash: list.o hash.o
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) hash_table.o linked_list.o -o hash 

test: list.o hash.o unit.o
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) $(CUNIT_LINK) unittests.o hash_table.o linked_list.o -o test

freq.o: freq-count.c
	$(C_COMPILER) $(C_OPTIONS) -c freq-count.c

unit.o: unittests.c
	$(C_COMPILER) $(C_OPTIONS) -c unittests.c

hash.o: hash_table.c hash_table.h common.h
	$(C_COMPILER) $(C_OPTIONS) -c hash_table.c

list.o: linked_list.c linked_list.h iterator.h common.h
	$(C_COMPILER) $(C_OPTIONS) -c linked_list.c


