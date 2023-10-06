# Copyright © 2023 Brian Pomerantz. All Rights Reserved.

CC = g++
CFLAGS = -Wall -std=c++2a -O3 -fopenmp
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)
