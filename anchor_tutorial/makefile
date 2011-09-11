binary : objects
	g++ -Lphoenix/build `pkg-config --libs gtk+-2.0` -oMainFile MainFile.o anchor-layout.o -lphoenix_gtk -lX11

objects : MainFile.cpp anchor-layout.cpp
	g++ -Iphoenix -I. -O0 -g3  -Wall -c -fmessage-length=0 -std=c++0x `pkg-config --cflags gtk+-2.0` -DPHOENIX_GTK MainFile.cpp anchor-layout.cpp

clean :
	rm MainFile.o anchor-layout.o MainFile

