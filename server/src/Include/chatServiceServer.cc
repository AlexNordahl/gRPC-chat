#include "chatServiceServer.h"

::grpc::Status ChatServiceServer::Chat([[maybe_unused]]::grpc::ServerContext *context, ::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent> *stream)
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
                
                ChatEvent ev {};
                UserList* userList = ev.mutable_user_list();
                for (const auto& u : usernames)
                    userList->add_usernames(u);

                usernames_mutex.unlock();
                
                broadcast(stream, ev, false);
                break;
            }
            case ChatEvent::kUserList: break;
            case ChatEvent::PAYLOAD_NOT_SET: break;
        }
    }

    return grpc::Status::OK;
}

void ChatServiceServer::broadcast(::grpc::ServerReaderWriter<::ChatEvent, ::ChatEvent> *stream, ChatEvent &event, bool echoMessage)
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto s : clients)
    {   
        if (!echoMessage && s == stream)
            continue;

        s->Write(event);
    }
}

std::string ChatServiceServer::convertTime(const google::protobuf::Timestamp &timeStamp, const std::string &format)
{
    time_t t = static_cast<time_t>(timeStamp.seconds());
    std::tm tm{};
    localtime_r(&t, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, format.c_str());

    return oss.str();
}