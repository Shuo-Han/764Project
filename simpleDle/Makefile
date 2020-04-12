# .o files are objects. They are the output of the compiler and input to the linker/librarian.
# .a files are archives. They are groups of objects or static libraries and are also input into the linker.
# -I. is included so that gcc will look in the current directory (.) for including .h file. 
# -c says not to run the linker. Then the output consists of object files output by the assembler.
CC = g++
CFLAGS = -std=c++17 -Wall -Werror
# LIB = lib

# get littleBadger_main
all: threadManager.o badgerThread.o lockManager.o txnManager.o sysStructure.o 
	$(CC) $(CFLAGS) threadManager.o badgerThread.o lockManager.o txnManager.o sysStructure.o -o littleBadger_main

# specify that threadManager.o is created based on threadManager.*
# g++ -std=c++11 -Wall -Werror -c -I. threadManager.cpp
threadManager.o: threadManager.* 
	$(CC) $(CFLAGS) -c -I. threadManager.cpp 

# specify that badgerThread.o is created based on badgerThread.*
# g++ -std=c++11 -Wall -Werror -c -I. badgerThread.cpp
badgerThread.o: badgerThread.* 
	$(CC) $(CFLAGS) -c -I. badgerThread.cpp 

# specify that lockManager.o is created based on lockManager.*
# g++ -std=c++11 -Wall -Werror -c -I. lockManager.cpp
lockManager.o: lockManager.*  
	$(CC) $(CFLAGS) -c -I. lockManager.cpp

# specify that txnManager.o is created based on txnManager.*
# g++ -std=c++11 -Wall -Werror -c -I. txnManager.cpp
txnManager.o: txnManager.*  
	$(CC) $(CFLAGS) -c -I. txnManager.cpp

# specify that sysStructure.o is created based on sysStructure.*
# g++ -std=c++11 -Wall -Werror -c -I. sysStructure.cpp
sysStructure.o: sysStructure.*  
	$(CC) $(CFLAGS) -c -I. sysStructure.cpp

# specify that globals.o is created based on globals.*
# g++ -std=c++11 -Wall -Werror -c -I. globals.h
# globals.o: globals.*  
# 	$(CC) $(CFLAGS) -c -I. globals.cpp


