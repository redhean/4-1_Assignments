# Makefile
TARGET=ex5
CXX=gcc
OBJECTS=p5.o
$(TARGET):$(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)
p5.o: p5.c
	$(CXX) -c p5.c
clean:
	rm $(OBJECTS) $(TARGET)
