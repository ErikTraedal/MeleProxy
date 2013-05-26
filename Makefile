INCLUDES =
 
CC = gcc -g -Wno-deprecated
CFLAGS = -c $(INCLUDES) -D DEBUG
LIBS = -ludev

all : mele

mele : mele.o udev.o uinput.o
	$(CC) mele.o udev.o uinput.o -o mele $(LIBS)
mele.o : mele.c
	$(CC) $(CFLAGS) mele.c 
udev.o : udev.c
	$(CC) $(CFLAGS) udev.c

uinput.o : uinput.c
	$(CC) $(CFLAGS) uinput.c
clean :
	rm -f *.o
