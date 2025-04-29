# Variables
CC = gcc
CFLAGS = -Wall -Ofast -fopenmp 
TARGET = lab
SOURCES = main.c matrix.c bfm.c
TEST_TARGET = lab_test
TEST_SOURCES = test.c points.c
OBJECTS = $(SOURCES:.c=.o)

# Rule to build the target
$(TARGET): src/$(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

# Run rule
run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean rule
clean:
	rm -f $(OBJECTS) $(TARGET)
