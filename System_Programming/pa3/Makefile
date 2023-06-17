#Makefile
CC = gcc
TARGET_SERVER := pa3_server
TARGET_CLIENT := pa3_client

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): $(TARGET_SERVER).o
	$(CC) $< -o $@ -lpthread

$(TARGET_CLIENT): $(TARGET_CLIENT).o
	$(CC) $< -o $@ -lpthread

$(TARGET_SERVER).o: $(TARGET_SERVER).c
	$(CC) -c $< -o $@

$(TARGET_CLIENT).o: $(TARGET_CLIENT).c
	$(CC) -c $< -o $@

.PHONY: clean
clean: 
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT) $(TARGET_SERVER).o $(TARGET_CLIENT).o


