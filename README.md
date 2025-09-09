# Simple gRPC Chat Application

This project is a simple terminal-based chat client implemented in **C++** using **gRPC** and **Protocol Buffers**.  
It demonstrates how to build a bidirectional streaming application where multiple users can exchange messages in real time.

## Features
- Terminal chat interface  
- User login with username validation  
- Real-time message exchange over gRPC streams  
- Message timestamps

## Project Structure
- `proto/` – gRPC service definition
- `include/` – helper classes (`SimpleChatUI`, `timeFunctions`, `welcomeScreen`)  
- `main.cpp` – entry point with client logic  

## Requirements
- C++20 compiler 
- gRPC and Protocol Buffers installed
- ncurses installed
- CMake (recommended for building)  

## Build Instructions
```bash
mkdir build && cd build
cmake ..
make
```
## Planned Features
- Messages stored in database (SQLite)
- Ability to scroll back to prevoius messages
- Colored usernames and UI
