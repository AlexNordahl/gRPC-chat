#include <iostream>
#include <grpcpp/grpcpp.h>
#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"

int main()
{
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = ChatService::NewStub(channel);

    std::cout << "Enter your username: ";
    std::string username;
    std::getline(std::cin >> std::ws, username);
    
    grpc::ClientContext context;

    auto stream = stub->Chat(&context);

    while (true)
    {
        std::cout << "Message: ";
        std::string text;
        std::getline(std::cin >> std::ws, text);

        ChatMessage msg;

        stream->Read(&msg);

        std::cout << "[" << msg.username() << "]: " << msg.text() << "\n";

        ChatMessage message;
        message.set_username(username);
        message.set_text(text);

        stream->Write(message);
    }

    return 0;
}