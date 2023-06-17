#Makefile
TARGET=pa1
CXX=gcc
OBJECTS=main.o
CFLAGS=-I.

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)
main.o:main.c header.h
	$(CXX) $(CFLAGS) -c main.c
clean:
	rm $(OBJECTS) $(TARGET)
