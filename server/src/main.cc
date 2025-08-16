#include <iostream>
#include <grpcpp/grpcpp.h>
#include <vector>
#include <thread>
#include <chrono>
#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"

class ChatServiceImpl : public ChatService::Service
{
private:
    std::mutex clients_mutex;
    std::vector<::grpc::ServerReaderWriter<::ChatMessage, ::ChatMessage>*> clients;

public:
    ::grpc::Status Chat([[maybe_unused]] ::grpc::ServerContext* context, ::grpc::ServerReaderWriter<::ChatMessage, ::ChatMessage>* stream) override
    {
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(stream);
        }

        ChatMessage incoming; 
        while (stream->Read(&incoming))
        {
            std::cout << "[" << incoming.username() << "]: " << incoming.text() << "\n";

            std::lock_guard<std::mutex> lock(clients_mutex);
            for (auto s : clients)
            {   
                if (s != stream)
                    s->Write(incoming);
            }
        }

        return grpc::Status::OK;
    }
};

int main()
{
    const std::string address {"localhost:50051"};

    ChatServiceImpl service;

    grpc::ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());

    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

    std::cout << "Server started\n";

    server->Wait();

    return 0;
}