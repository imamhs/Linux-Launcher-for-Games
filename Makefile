# Makefile
# Copyright (C) 2014-2015 Md. Imam Hossain
# For conditions of distribution and use, see copyright notice in License.txt
#

Linux Launcher for NFS PU: main.o misc.o
	g++ -g -o "Linux Launcher for NFS PU" main.o misc.o `pkg-config --cflags --libs gtk+-2.0`
main.o: main.cpp
	g++ -g -c main.cpp `pkg-config --cflags --libs gtk+-2.0`
misc.o: misc.cpp
	g++ -g -c misc.cpp `pkg-config --cflags --libs gtk+-2.0`
clean:
	rm -f *.o "Linux Launcher for NFS PU"
run: Linux Launcher for NFS PU
	./"Linux Launcher for NFS PU"
debug: NFS Linux Installer
	gdb "Linux Launcher for NFS PU"
