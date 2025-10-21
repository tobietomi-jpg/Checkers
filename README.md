# Checkers
# BitBoard Checkers

A complete checkers game implementation using bitboard techniques for efficient bit manipulation, developed for CS 3503 Computer Organization and Architecture.

## Project Overview

This project demonstrates advanced bit manipulation skills by implementing a fully functional checkers game where the board state is represented using 64-bit integers (bitboards). Each bit in the integer represents a square on the 8×8 checkerboard.

## Features

### Phase 1: Bit Manipulation Library
- **32-bit operations**: Complete set of bit manipulation functions for 32-bit integers
- **64-bit operations**: Extended functions for 64-bit board representation
- **Comprehensive testing**: Standalone test suite for all bit operations

### Phase 2: Checkers Game
- **Bitboard representation**: Efficient game state using 64-bit integers
- **Complete game rules**: Moves, captures, king promotion, and win detection
- **Multi-capture sequences**: Support for complex jump chains
- **Interactive interface**: Clean ASCII display with coordinate system

## File Structure
bitboard-checkers/
├── README.md
├── Makefile
├── bit_ops.h # Header file with function declarations
├── phase1.c # Phase 1: Bit manipulation testing
├── phase2.c # Phase 2: Complete checkers game
└── sample_output.txt # Example game session

## Build Instructions

**gcc Phase2.c -o Phase2
./Phase2**

### Prerequisites
- GCC compiler
- Make build system
- Standard C library

### Compilation
```bash
# Build everything (default)
make

# Build only Phase 1 bit manipulation demo
make phase1

# Build only the checkers game
make game

# Clean build files
make clean

Gameplay Instructions
Board Representation: The 8×8 board uses only 32 playable squares (dark squares), numbered 1-32

Pieces:

Player 1: b (regular), B (king) - moves downward

Player 2: w (regular), W (king) - moves upward

Move Notation:

Simple move: 12-16 (move from square 12 to 16)

Capture: 12x19 or 12x19x28 for multiple captures

# Rules:

Regular pieces move forward diagonally

Kings can move in both directions

Captures are mandatory when available

Pieces become kings when reaching the opposite end
