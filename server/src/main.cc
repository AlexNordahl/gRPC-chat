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
    std::vector<::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent>*> clients;

public:
    ::grpc::Status Chat(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::ChatEvent, ::ChatEvent>* stream) override
    {
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(stream);
        }

        ChatEvent incoming; 
        while (stream->Read(&incoming))
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            for (auto s : clients)
            {   
                if (s != stream)
                    s->Write(incoming);
            }
        }

        return grpc::Status::OK;
    }

    std::string convertTime(const google::protobuf::Timestamp& timeStamp, const std::string& format)
    {
        time_t t = static_cast<time_t>(timeStamp.seconds());
        std::tm tm{};
        localtime_r(&t, &tm);

        std::ostringstream oss;
        oss << std::put_time(&tm, format.c_str());

        return oss.str();
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

    server->Wait();

    return 0;
}