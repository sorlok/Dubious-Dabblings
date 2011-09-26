#Our phoenix_ext library.
lib: anchor-layout.o  image-icon.o  thumbnail-layout.o
	ar rcs build/libphoenix_ext.a anchor-layout.o  image-icon.o  thumbnail-layout.o

image-icon.o: image-icon.cpp image-icon.hpp
	g++ -std=gnu++0x -I. -I../phoenix -O3 -fomit-frame-pointer -o image-icon.o -c image-icon.cpp 

anchor-layout.o: anchor-layout.cpp anchor-layout.hpp scapegoat.hpp scapegoat-display.hpp fastlog.hpp
	g++ -std=gnu++0x -DANCHOR_LAYOUT_ERRORS_ON -I. -I../phoenix -O3 -fomit-frame-pointer -o anchor-layout.o -c anchor-layout.cpp

thumbnail-layout.o: thumbnail-layout.cpp thumbnail-layout.hpp scapegoat.hpp scapegoat-display.hpp fastlog.hpp
	g++ -std=gnu++0x -I. -I../phoenix -O3 -fomit-frame-pointer -o thumbnail-layout.o -c thumbnail-layout.cpp

clean:
	rm -f thumbnail-layout.o image-icon.o anchor-layout.o build/libphoenix_ext.a

