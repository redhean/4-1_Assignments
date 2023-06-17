#Makefile

TARGET=p7
CXX=gcc
OBJECTS=main.o
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)
main.o: main.c
	$(CXX) -c main.c
clean:
	rm $(OBJECTS) $(TARGET)
