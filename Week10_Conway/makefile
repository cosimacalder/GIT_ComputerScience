CFLAGS = `sdl2-config --cflags` -Wall -pedantic -std=c99 -lm
INCS = neillsdl2.h 
TARGET = myconway
SOURCES =  neillsdl2.c $(TARGET).c
LIBS =  `sdl2-config --libs`
CC = gcc


all: $(TARGET)

$(TARGET): $(SOURCES) $(INCS)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	$(TARGET) 
