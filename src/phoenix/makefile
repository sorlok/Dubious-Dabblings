phoenix: phoenix.o
	ar rcs build/libphoenix_ext.a phoenix.o

phoenix.o: phoenix.cpp core/core.hpp core/core.cpp
	g++ -std=gnu++0x -I. -O3 -fomit-frame-pointer -c phoenix.cpp -DPHOENIX_GTK `pkg-config --cflags gtk+-2.0`

