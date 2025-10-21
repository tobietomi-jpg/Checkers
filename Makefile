# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 

# Targets
PHASE1_TARGET = phase1_bit_ops
PHASE2_TARGET = checkers

# Phase 1 source files
PHASE1_SRCS = project.c
PHASE1_OBJS = $(PHASE1_SRCS:.c=.o)

# Phase 2 source files  
PHASE2_SRCS = phase2.c
PHASE2_OBJS = $(PHASE2_SRCS:.c=.o)

# Default target - build both phases
all: $(PHASE1_TARGET) $(PHASE2_TARGET)

# Phase 1: Bit operations test program
$(PHASE1_TARGET): $(PHASE1_OBJS)
	$(CC) $(CFLAGS) -o $(PHASE1_TARGET) $(PHASE1_OBJS) $(LDFLAGS)

# Phase 2: Checkers game
$(PHASE2_TARGET): $(PHASE2_OBJS)
	$(CC) $(CFLAGS) -o $(PHASE2_TARGET) $(PHASE2_OBJS) $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(PHASE1_OBJS) $(PHASE2_OBJS) $(PHASE1_TARGET) $(PHASE2_TARGET)

# Run Phase 1 tests
test-phase1: $(PHASE1_TARGET)
	./$(PHASE1_TARGET)

# Run Phase 2 game
run-game: $(PHASE2_TARGET)
	./$(PHASE2_TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: clean all

# Release build (optimized)
release: CFLAGS += -O2
release: clean all

# Individual build targets
phase1: $(PHASE1_TARGET)
phase2: $(PHASE2_TARGET)

.PHONY: all clean test-phase1 run-game debug release phase1 phase2
