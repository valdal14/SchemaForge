# Compiler and Flags
CC = gcc
CFLAGS = -O2 -W -Wall

# Target executable
TARGET = sfengine.out

# Source files
SRCS = main.c parser.c types.c

# Default build (Production, no debug output)
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Debug build (Includes visual tokenization output)
debug:
	$(CC) $(CFLAGS) -DDEBUG $(SRCS) -o $(TARGET)

# Clean up build artifacts
clean:
	rm -f $(TARGET)
