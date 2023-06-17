# Makefile

TARGET=p2.out
CXX=gcc
OBJECTS=main.o plus.o minus.o
$(TARGET):$(OBJECTS)
	$(CXX) -o $@ $^

main.o:main.c
	$(CXX) -c main.c
plus.o:plus.c
	$(CXX) -c plus.c
minus.o:minus.c
	$(CXX) -c minus.c
clean:
	rm $(OBJECTS) $(TARGET)

