#include <iostream>
#include <grpcpp/grpcpp.h>
#include <vector>
#include <thread>
#include <chrono>
#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"

class ChatServiceImpl : public ChatService::Service
{
public:
    ::grpc::Status Chat(::grpc::ServerContext* context, ::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent>* stream) override
    {
        clients_mutex.lock();
        clients.push_back(stream);
        clients_mutex.unlock();

        ChatEvent incoming; 
        while (stream->Read(&incoming))
        {
            switch (incoming.payload_case())
            {
                case ChatEvent::kChatMessage:
                {
                    broadcast(stream, incoming, false);
                    break;
                }
                case ChatEvent::kUserJoined:
                {
                    usernames_mutex.lock();
                    usernames.insert(incoming.user_joined().username());

                    ChatEvent ev {};
                    UserList* userList = ev.mutable_user_list();
                    for (const auto& u : usernames)
                        userList->add_usernames(u);
                    usernames_mutex.unlock();
                    
                    broadcast(stream, ev, true);
                    break;
                }
                case ChatEvent::kUserLeft:
                {
                    usernames_mutex.lock();
                    auto it {usernames.find(incoming.user_left().username())};
                    usernames.erase(it);
                    usernames_mutex.unlock();
                    break;
                }
            }
        }

        return grpc::Status::OK;
    }
    
private:
    std::mutex clients_mutex {};
    std::mutex usernames_mutex {};
    std::vector<::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent>*> clients {};
    std::unordered_set<std::string> usernames {};

    void broadcast(::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent>* stream, ChatEvent& event, bool echoMessage)
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto s : clients)
        {   
            if (!echoMessage && s == stream)
                continue;

            s->Write(event);
        }
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