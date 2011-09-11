gtk: phoenix_gtk.o
	mkdir -p build
	ar rcs build/libphoenix_gtk.a phoenix_gtk.o

phoenix_gtk.o: phoenix.cpp core/core.hpp core/core.cpp
	g++ -std=gnu++0x -I. -O3 -fomit-frame-pointer -o phoenix_gtk.o -c phoenix.cpp -DPHOENIX_GTK `pkg-config --cflags gtk+-2.0`

clean:
	rm phoenix_gtk.o build/libphoenix_gtk.a 

