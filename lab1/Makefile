# Variables
CC = gcc
CFLAGS = -Wall -Ofast -fopenmp
TARGET = lab
TEST_SRCS = test.c
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)

# Rule to build the target
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to build the target
test_prg: $(TEST_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test: test_prg
	./test_prg
	rm -f test_prg

test2_prg: test2.c
	$(CC) $(CFLAGS) -o test2_prg test2.c

test2: test2_prg
	./test2_prg
	rm -f test2_prg

racism_prg: racism.c
	$(CC) $(CFLAGS) -o racism_prg racism.c

racism: racism_prg
	./racism_prg
	rm -f racism_prg

# Run rule
run: $(TARGET)
	./$(TARGET)

# Clean rule
clean:
	rm -f $(OBJECTS) $(TARGET)
