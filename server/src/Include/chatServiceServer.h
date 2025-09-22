#ifndef CHAT_SERVICE_SERVER
#define CHAT_SERVICE_SERVER

#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include <vector>
#include <thread>
#include <chrono>

class ChatServiceServer : public ChatService::Service
{
public:
    ::grpc::Status Chat(::grpc::ServerContext* context, ::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent>* stream) override;
    
private:
    std::mutex clients_mutex {};
    std::mutex usernames_mutex {};
    std::vector<::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent>*> clients {};
    std::unordered_set<std::string> usernames {};

    void broadcast(::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent>* stream, ChatEvent& event, bool echoMessage);
    std::string convertTime(const google::protobuf::Timestamp& timeStamp, const std::string& format);
};

#endif