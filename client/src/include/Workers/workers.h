#ifndef WORKERS_H
#define WORKERS_H

#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include "../UI/simpleChatUI.h"
#include "../UI/welcomeScreen.h"
#include "../Utility/timeFunctions.h"
#include "../UI/theme.h"
#include <grpcpp/grpcpp.h>
#include <thread>
#include <mutex>
#include <condition_variable>

using clientReaderWriter = std::shared_ptr<grpc::ClientReaderWriter<ChatEvent, ChatEvent>>;

std::condition_variable writeCondVar;
std::mutex toSendMutex;
std::mutex incomingMutex;
const std::string quitCommand {"/quit"};

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatEvent>& incomingQueue, std::string_view username)
{
    SimpleChatUI UI {};
    std::string input {};

    while (true)
    {
        UI.handleInput();

        input = UI.takeInput();
        if (!input.empty())
        {
            if (input == quitCommand)
            {   
                { std::lock_guard<std::mutex> lg(toSendMutex); toSendQueue.push(input); }
                writeCondVar.notify_one();
                break;
            }

            { std::lock_guard<std::mutex> lg(toSendMutex); toSendQueue.push(input); }
            writeCondVar.notify_one();
            UI.addMessage(getTimestamp() + "[" + username.data() + "]: " + input, Color::White);
        }

        std::lock_guard<std::mutex> lock(incomingMutex);
        while (!incomingQueue.empty())
        {
            ChatEvent event = incomingQueue.front();
            incomingQueue.pop();

            switch (event.payload_case())
            {
                case ChatEvent::kChatMessage:
                {
                    const ChatMessage& msg = event.chat_message();
                    UI.addMessage(convertProtoTime(msg.sent_at()) + "[" + msg.username() + "]: " + msg.text(), msg.color());
                    break;
                }
                case ChatEvent::kUserList:
                {
                    std::vector<std::string> usernames {};
                    for (const auto& name : event.user_list().usernames())
                        usernames.push_back(name);
                    UI.UpdateUsers(usernames);
                    break;
                }
                case ChatEvent::kUserJoined:
                {
                    const UserJoined& joined = event.user_joined();
                    UI.addMessage(joined.username() + " has joined the server", joined.color());
                    break;
                }
                case ChatEvent::kUserLeft:
                {
                    const UserLeft& left = event.user_left();
                    UI.addMessage(left.username() + " has left the server...", left.color());
                    break;
                }
                case ChatEvent::PAYLOAD_NOT_SET: break;
            }
        }
    }
}

static inline void userJoined(ChatEvent& ev, clientReaderWriter& stream, const std::string_view username)
{
    UserJoined* joined {ev.mutable_user_joined()};
    joined->mutable_joined_at()->set_seconds(std::time(nullptr));
    joined->set_username(std::string(username));
    joined->set_color(Color::Yellow);
    stream->Write(ev);
}

static inline void userLeft(ChatEvent& ev, clientReaderWriter& stream, const std::string_view username)
{
    UserLeft* left {ev.mutable_user_left()};
    left->mutable_left_at()->set_seconds(std::time(nullptr));
    left->set_username(std::string(username));
    left->set_color(Color::Red);
    stream->Write(ev);
}

void writeThread(std::queue<std::string>& toSendQueue, clientReaderWriter& stream, const std::string_view username)
{
    ChatEvent ev {};

    userJoined(ev, stream, username);

    while (true)
    {
        std::unique_lock<std::mutex> ul(toSendMutex);
        writeCondVar.wait(ul, [&]{ return !toSendQueue.empty(); });

        std::string text = toSendQueue.front();
        toSendQueue.pop();
        ul.unlock();

        if (text == quitCommand)
        {
            userLeft(ev, stream, username);
            stream->WritesDone();
            break;      
        }

        ChatMessage* msg = ev.mutable_chat_message();
        auto* ts = msg->mutable_sent_at();
        ts->set_seconds(std::time(nullptr));
        msg->set_username(std::string(username));
        msg->set_text(text);
        msg->set_color(Color::White);

        if (!stream->Write(ev))
            break;
    }
}

void readThread(std::queue<ChatEvent>& incomingQueue, clientReaderWriter& stream)
{
    ChatEvent ev {};
    while (true)
    {
        if (!stream->Read(&ev))
            break;

        std::lock_guard<std::mutex> lock(incomingMutex);
        incomingQueue.push(ev);
    }
}

#endif
