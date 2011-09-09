#Default target is GTK. Windows and QT targets provided, but not (yet) tested.

gtk: phoenix_gtk.o
	ar rcs build/libphoenix_ext_gtk.a phoenix_gtk.o

win: phoenix_win.o
	i586-mingw32msvc-g++ -mwindows -s phoenix_win.o phoenix-winres.o -lkernel32 -luser32 -lgdi32 -ladvapi32 -lcomctl32 -lcomdlg32 -lshell32 -lole32

qt: phoenix_qt.o
	ar rcs build/libphoenix_ext_qt.a phoenix_qt.o

phoenix_gtk.o: phoenix.cpp core/core.hpp core/core.cpp
	g++ -std=gnu++0x -I. -O3 -fomit-frame-pointer -o phoenix_gtk.o -c phoenix.cpp -DPHOENIX_GTK `pkg-config --cflags gtk+-2.0`

phoenix_win.o: phoenix.cpp core/core.hpp core/core.cpp phoenix-winres.o
	i586-mingw32msvc-g++ -std=gnu++0x -I. -I/usr/i586-mingw32msvc/include -O3 -fomit-frame-pointer -DPHOENIX_WINDOWS  -o phoenix_win.o -c phoenix.cpp

phoenix-winres.o: windows/phoenix.rc
	i586-mingw32msvc-windres windows/phoenix.rc phoenix-winres.o

phoenix_qt.o: phoenix.cpp core/core.hpp core/core.cpp phoenix-qt-moc
	g++ -std=gnu++0x -I. -O3 -fomit-frame-pointer -o phoenix_qt.o -c phoenix.cpp -DPHOENIX_QT `pkg-config --cflags QtCore QtGui`

phoenix-qt-moc: 
	moc -i -o qt/qt.moc qt/qt.moc.hpp

clean:
	rm -f phoenix_gtk.o phoenix_win.o phoenix_qt.o phoenix-winres.o build/libphoenix_ext_gtk.a build/libphoenix_ext_win.a build/libphoenix_ext_qt.a




