# gRPC Chat

## Motivation

Idea of creating terminal based chat application was very exciting.
The goal of this project was also to get better at programming in C++ using modern tools. I wanted to improve my ability to structure C++ projects properly, including good practices with CMake.
This project also served as an opportunity to improve my skills with multithreading and synchronization primitives (**std::mutex**, **std::condition_variable**).
Project also demonstrates the classic **producer-consumer concurrency problem**, implemented with message queues.

## Example

Below is an example screenshot of the chat client in action:

![Chat Example](https://github.com/AlexNordahl/gRPC-chat/blob/main/visual-examples/example.png)

## Features
- Terminal-based chat interface  
- Real-time message exchange over gRPC streams  
- Message timestamps  
- Colored messages

## Project Structure
- **proto/** – gRPC service definition and generated code (`chat.proto`, `chat.pb.h`, `chat.grpc.pb.h`)  
- **include/** – helper classes (`SimpleChatUI`, `timeFunctions`, `welcomeScreen`)  
- **main.cpp** – entry point with client logic  

## Requirements
- C++20 compiler
- gRPC and Protocol Buffers installed
- ncurses installed
- CMake (recommended for building)


## Future Improvements

I wanted to keep this project simple and not let feature creep in but here are some ideas for the future:
- Improved concurrency using a **multithreaded queue** for better performance
- Persistent message history (e.g., storing logs in a database)
- Encrypted communication for better security
- Tests and gRPC performance benchmarking
- Heavier optimization

## Build Instructions

```bash
docker build -t grpc-chat .

docker run -it --rm grpc-chat /bin/bash

# inside container
cd mnt/server/build
cmake ..
make

# go back to mnt
cd client/build
cmake ..
make
```