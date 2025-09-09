# Simple gRPC Chat Client

This project is a simple terminal-based chat client implemented in **C++** using **gRPC** and **Protocol Buffers**.  
It demonstrates how to build a bidirectional streaming application where multiple users can exchange messages in real time.

## Features
- Terminal chat interface  
- User login with username validation  
- Real-time message exchange over gRPC streams  
- Message timestamps  
- Multithreading with `std::jthread`, `std::mutex`, and `std::condition_variable`  

## Project Structure
- `proto/` – gRPC service definition and generated code (`chat.proto`, `chat.pb.h`, `chat.grpc.pb.h`)  
- `include/` – helper classes (`SimpleChatUI`, `timeFunctions`, `welcomeScreen`)  
- `main.cpp` – entry point with client logic  

## Requirements
- C++20 compiler (e.g. GCC 11+ or Clang 12+)  
- gRPC and Protocol Buffers installed
- ncurses installed
- CMake (recommended for building)  

## Build Instructions
```bash
mkdir build && cd build
cmake ..
make