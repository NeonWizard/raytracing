# -g : inclue debugging symbols

# $@ : the target
# $^ : all dependencies
# $< : first dependency

CXXFLAGS := -Wall -Wno-missing-braces -Werror -g -std=c++11
CXX := g++

all: main

run: main
	./main > image.ppm

main: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o main valgrind.*

memtest: main
	rm -f valgrind.log
	valgrind --leak-check=full --track-fds=yes --log-file=valgrind.log ./main
	cat main
