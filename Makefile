CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra 
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

asm: asm.o
	g++ -o asm asm.o

asm.o: asm.cpp
	g++ -c asm.cpp $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o asm
