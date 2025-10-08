# COMP345-Risk-Game-2025

A simplified Risk game compatible with Warzone rules and maps. This implementation includes a complete game engine with state management, map loading/validation, player management, order processing, and card mechanics.

## Project Overview

This project implements Assignment 1 for COMP345 (Advanced Program Design with C++) featuring:
- **Part 1**: Map loading, validation, and representation
- **Part 2**: Player management with hand ownership and order generation
- **Part 3**: Order system with polymorphic order types and execution
- **Part 4**: Card system with deck, hand, and card playing mechanics
- **Part 5**: Game engine with state machine and command processing

## System Requirements

- **C++ Compiler**: C++17 compatible compiler (g++, clang++)
- **Operating System**: macOS, Linux, or Windows with appropriate C++17 support
- **Build Tools**: Standard make tools or manual compilation
- **File System**: C++17 filesystem support required for map loading

## Building and Running

### Quick Start (Recommended)

The project includes VS Code tasks for easy building and running:

1. **Build the project**:
   ```bash
   g++ -g -std=c++17 -Wall -Wextra -I./include -o warzone_test drivers/*.cpp src/*.cpp
   ```

2. **Run the program**:
   ```bash
   ./warzone_test
   ```

### Using VS Code Tasks (if available)

If using VS Code, you can use the predefined tasks:
- **Build**: `Ctrl+Shift+P` → "Tasks: Run Task" → "build"
- **Run**: `Ctrl+Shift+P` → "Tasks: Run Task" → "run"
- **Clean**: `Ctrl+Shift+P` → "Tasks: Run Task" → "clean"

## Project Structure

```
COMP345-Risk-Game-2025/
├── README.md              # This file
├── assets/
│   ├── maps/              # Map files (.map format)
│   └── Pictures/          # Map visualization images
├── drivers/               # Test driver files
│   ├── MainDriver.cpp     # Main program entry point
│   ├── MapDriver.cpp      # Map functionality tests
│   ├── PlayerDriver.cpp   # Player functionality tests
│   ├── OrdersDriver.cpp   # Orders functionality tests
│   ├── CardsDriver.cpp    # Cards functionality tests
│   └── GameEngineDriver.cpp # Game engine tests
├── include/               # Header files
│   ├── Map.h
│   ├── Player.h
│   ├── Orders.h
│   ├── Cards.h
│   └── GameEngine.h
└── src/                   # Implementation files
    ├── Map.cpp
    ├── Player.cpp
    ├── Orders.cpp
    ├── Cards.cpp
    └── GameEngine.cpp
```

## What the Program Does

When you run `./warzone_test`, the program executes test drivers that demonstrate all assignment requirements:

### 1. Map Loading and Validation Tests
- Loads and validates multiple map files from `assets/maps/`
- Tests map connectivity and continent validation
- Demonstrates copy constructors and assignment operators
- Interactive map selection interface

### 2. Player Management Tests
- Creates players with hand ownership
- Demonstrates `toDefend()` and `toAttack()` methods
- Shows `issueOrder()` functionality
- Tests memory management and Rule of Three compliance

### 3. Orders System Tests  
- Creates all order types: Deploy, Advance, Bomb, Blockade, Airlift, Negotiate
- Demonstrates OrdersList management (add, remove, move operations)
- Shows polymorphic order execution
- Tests order validation and effects

### 4. Cards System Tests
- Creates deck with all card types
- Demonstrates card drawing from deck to player hand
- Shows card playing that generates corresponding orders
- Tests proper card return to deck after playing

### 5. Game Engine Tests
- Interactive state machine demonstration
- Command processing and state transitions
- Error handling for invalid commands
- Game flow from Start to End states

## Expected Output

The program will run all test drivers sequentially and display:
-  Successful operations and test results
-  Object states and relationships
-  Interactive prompts (for map selection and game engine)
-  Error messages for invalid operations
-  Comprehensive test summaries

## Interactive Components

Some tests include interactive elements:
- **Map Selection**: Choose from available map files or exit
- **Game Engine**: Type commands to navigate game states
  - Commands: `loadmap`, `validatemap`, `addplayer`, etc.
  - Type `help` for valid commands in current state
  - Type `quit` or `exit` to finish testing

## Troubleshooting

### Compilation Issues
- Ensure C++17 support: Use `-std=c++17` flag
- Include path: Use `-I./include` flag
- All warnings: Use `-Wall -Wextra` flags for comprehensive error checking

### Runtime Issues
- **"No map files found"**: Ensure `assets/maps/` directory exists with `.map` files
- **"File not found"**: Check that you're running from the project root directory

### File System Requirements
- The program uses C++17 filesystem features for map file discovery
- Ensure your compiler and standard library support `std::filesystem`

## Assignment Compliance

This implementation fulfills all requirements for COMP345 Assignment 1:

- **Part 1**: Complete map loading, validation, and graph representation
- **Part 2**: Full player implementation with territory management
- **Part 3**: Comprehensive order system with all required order types
- **Part 4**: Complete card system with proper deck/hand mechanics
- **Part 5**: Full game engine with state machine implementation

All classes implement proper:
- Copy constructors and assignment operators (Rule of Three)
- Stream insertion operators
- Memory management (no memory leaks)
- Comprehensive test drivers demonstrating functionality

## Authors

- Andrew Pulsifer
- Matteo
- Chhay

Note: Roman did not contribute.  

## Version

Version 1.0 - October 2025
