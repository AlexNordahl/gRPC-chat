cmake_minimum_required(VERSION 3.20)
project(grpc_server)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Curses REQUIRED)

add_library(protolib proto/chat.proto)
target_link_libraries(protolib gRPC::grpc++)
target_include_directories(protolib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(TARGET protolib LANGUAGE cpp)
protobuf_generate(TARGET protolib LANGUAGE grpc
GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

add_executable(main
    src/main.cc
    src/include/UI/simpleChatUI.cc
    src/include/UI/welcomeScreen.cc
)

target_compile_features(main PRIVATE cxx_std_20)
target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(main PRIVATE protolib ${CURSES_LIBRARIES})