# -g : inclue debugging symbols

# $@ : the target
# $^ : all dependencies
# $< : first dependency

CXXFLAGS := -Wall -Wno-missing-braces -Werror -g -std=c++11
CXX := g++

all: main

run: main
	./main > image.ppm
	convert image.ppm "images/$$(date +'%Y-%m-%d %H-%M:%S').png"

main: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cpp color.h vec3.h ray.h hittable.h sphere.h hittable_list.h helper.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o main valgrind.*

memtest: main
	rm -f valgrind.log
	valgrind --leak-check=full --track-fds=yes --log-file=valgrind.log ./main
	cat main
