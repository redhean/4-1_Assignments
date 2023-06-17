#Makefile
TARGET=ex
CXX=gcc
OBJECTS=cron.o 
$(TARGET): $(OBJECTS)
		$(CXX) -o $(TARGET) $(OBJECTS)
cron.o:cron.c 
		$(CXX) -c cron.c 
clean:
		rm $(OBJECTS) $(TARGET)