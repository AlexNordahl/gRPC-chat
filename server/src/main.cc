#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include "include/chatServiceServer.h"
#include <grpcpp/grpcpp.h>

int main()
{
    const std::string address {"localhost:50051"};
    ChatServiceServer service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

    server->Wait();
}