# VARIABLES
C_COMPILER     = gcc
C_OPTIONS      = -Wall -pedantic -g
C_LINK_OPTIONS = -lm 
CUNIT_LINK     = -lcunit


hash: list.o hash.o
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) hash_table.o linked_list.o -o hash 

hash.o: hash_table.c hash_table.h common.h
	$(C_COMPILER) $(C_OPTIONS) -c hash_table.c

list.o: linked_list.c linked_list.h common.h
	$(C_COMPILER) $(C_OPTIONS) -c linked_list.c


