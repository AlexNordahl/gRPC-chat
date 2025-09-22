# gRPC Chat

## 📸 Example

Below is an example screenshot of the chat client in action:

![Chat Example](https://github.com/AlexNordahl/gRPC-chat/visual-examples/example.png)

This project is a simple **terminal-based chat client** implemented in **C++** using **gRPC**.
It demonstrates how to build a **bidirectional streaming application**, where multiple users can exchange messages in real time.

## ✨ Features
- Terminal-based chat interface  
- Real-time message exchange over gRPC streams  
- Message timestamps  
- Colored messages

## 📂 Project Structure
- **proto/** – gRPC service definition and generated code (`chat.proto`, `chat.pb.h`, `chat.grpc.pb.h`)  
- **include/** – helper classes (`SimpleChatUI`, `timeFunctions`, `welcomeScreen`)  
- **main.cpp** – entry point with client logic  

## ⚙️ Requirements
- C++20 compiler
- gRPC and Protocol Buffers installed
- ncurses installed
- CMake (recommended for building)

## 🛠️ Build Instructions

```bash
mkdir build && cd build
cmake ..
make
```