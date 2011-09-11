binary : MainFile.o
	g++ -L"phoenix/build" `pkg-config --libs gtk+-2.0` MainFile MainFile.o

Mainfile.o : MainFile.cpp
	g++ -DPHOENIX_GTK -I"phoenix" -I"." -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x `pkg-config --cflags gtk+-2.0` MainFile.o MainFile.cpp

clean :
	rm MainFile.o MainFile

