# Makefile for SR Library Test

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INCLUDES = -Isr_lib/include
LIBS = -Lsr_lib/bin -lsr_lib
TARGET = test_sr_lib.exe
SOURCE = test_sr_lib.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	del /f $(TARGET)

run: $(TARGET)
	.\$(TARGET)

.PHONY: all clean run