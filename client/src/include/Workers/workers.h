#ifndef WORKERS_H
#define WORKERS

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

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatEvent>& incomingQueue, std::string username)
{
    SimpleChatUI UI {};
    std::string input {};

    while (true)
    {
        UI.handleInput();

        input = UI.takeInput();
        if (!input.empty())
        {
            {
                std::lock_guard<std::mutex> lg(toSendMutex);
                toSendQueue.push(input);
            }
            writeCondVar.notify_one();
            UI.addMessage(getTimestamp() + "[" + username + "]: " + input, Color::White);
        }

        {
            std::lock_guard<std::mutex> lock(incomingMutex);
            while (!incomingQueue.empty())
            {
                ChatEvent incomingMsg = incomingQueue.front();
                incomingQueue.pop();

                if (incomingMsg.payload_case() == ChatEvent::kChatMessage) {
                    const ChatMessage& msg = incomingMsg.chat_message();
                    UI.addMessage(convertProtoTime(msg.sent_at()) + "[" + msg.username() + "]: " + msg.text(), msg.color());
                }
            }
        }
    }
}

void writeThread(std::queue<std::string>& toSendQueue, clientReaderWriter& stream, const std::string_view username)
{
    while (true)
    {
        std::unique_lock<std::mutex> ul(toSendMutex);
        writeCondVar.wait(ul, [&]{ return !toSendQueue.empty(); });

        std::string text = toSendQueue.front();
        toSendQueue.pop();
        ul.unlock();
        ChatEvent ev {};
        ChatMessage* msg = ev.mutable_chat_message();
        auto* ts = msg->mutable_sent_at();
        ts->set_seconds(std::time(nullptr));
        msg->set_username(std::string(username));
        msg->set_text(text);
        msg->set_color(Color::White);

        if (!stream->Write(ev)) break;
    }
}

void readThread(std::queue<ChatEvent>& incomingQueue, clientReaderWriter& stream)
{
    while (true)
    {
        ChatEvent ev {};
        if (!stream->Read(&ev)) break;
        std::lock_guard<std::mutex> lock(incomingMutex);
        incomingQueue.push(ev);
    }
}

#endif