# -g : inclue debugging symbols

# $@ : the target
# $^ : all dependencies
# $< : first dependency

CXXFLAGS := -Wall -Wno-missing-braces -Werror -g -std=c++11 -O3 -march=native
CXX := g++

all: main

run: main
	./main > image.ppm
	convert image.ppm "images/$$(date +'%Y-%m-%d %H-%M-%S').png"

main: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

utility: color.h vec3.h ray.h utility.h camera.h rtw_stb_image.h
hittables: hittable.h hittable_list.h sphere.h moving_sphere.h aarect.h bvh.h
surfaces: material.h texture.h perlin.h
misc: aabb.h

main.o: main.cpp utility hittables surfaces misc
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o main valgrind.*

memtest: main
	rm -f valgrind.log
	valgrind --leak-check=full --track-fds=yes --log-file=valgrind.log ./main
	cat main
