#Makefile
TARGET=ex8
CXX=gcc
OBJECTS=p8.o
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)
p8.o: p8.c
	$(CXX) -c p8.c
clean:
	rm $(OBJECTS) $(TARGET)
