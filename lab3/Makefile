# Variables
CC = gcc
CFLAGS = -Wall -Ofast -fopenmp 
TARGET = lab
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)

# Rule to build the target
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

# Run rule
run: $(TARGET)
	./$(TARGET)

# Clean rule
clean:
	rm -f $(OBJECTS) $(TARGET)
