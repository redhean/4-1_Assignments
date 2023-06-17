#Makefile
TARGET=p14
CXX=gcc
OBJECTS=main.o
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) -lpthread
main.o: main.c
	$(CXX) -c main.c
clean:
	rm $(OBJECTS) $(TARGET)
