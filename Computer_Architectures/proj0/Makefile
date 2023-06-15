#Makefile

TARGET=riscv-sim
CXX=gcc
OBJECTS=main.o
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)
main.o: main.c
clean:
	rm $(OBJECTS) $(TARGET)
