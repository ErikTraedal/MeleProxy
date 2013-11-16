INCLUDES =
 
CC = gcc -g -Wno-deprecated
CFLAGS = -c $(INCLUDES)
LIBS = -ludev
PREFIX = /usr/local
TARGET = mele

.PHONY: all clean install uninstall

all : $(TARGET)

install: $(TARGET)
	install -m 0755 $(TARGET) $(PREFIX)/sbin
	
uninstall:
	-rm $(PREFIX)/sbin/$(TARGET)
	
$(TARGET): mele.o udev.o uinput.o
	$(CC) mele.o udev.o uinput.o -o mele $(LIBS)
	
mele.o : mele.c
	$(CC) $(CFLAGS) mele.c
	 
udev.o : udev.c
	$(CC) $(CFLAGS) udev.c

uinput.o : uinput.c
	$(CC) $(CFLAGS) uinput.c
	
clean :
	rm -f *.o $(TARGET)
