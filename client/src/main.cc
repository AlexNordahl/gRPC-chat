#include <iostream>
#include <grpcpp/grpcpp.h>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <condition_variable>
#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include "include/simpleChatUI.h"
#include "include/timeFunctions.h"
#include "include/welcomeScreen.h"

using clientReaderWriter = std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>>;

std::condition_variable writeCondVar;
std::mutex toSendMutex;
std::mutex incomingMutex;

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatMessage>& incomingQueue, std::string username);
void writeThread(std::queue<std::string>& toSendQueue, clientReaderWriter& stream, const std::string_view myUsername);
void readThread(std::queue<ChatMessage>& incomingQueue,  clientReaderWriter& stream);

int main()
{
    auto channel {grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())};
    auto stub {ChatService::NewStub(channel)};
    grpc::ClientContext context;
    clientReaderWriter stream {stub->Chat(&context)};

    std::queue<std::string> toSendQueue;
    std::queue<ChatMessage> incomingQueue;

    std::string username {};

    {
        WelcomeScreen ws;
        ws.setUsernameSizeBounds(4, 15);
        ws.start();
        if (!ws.username().empty())
            username = ws.username();
    }

    std::jthread t1(uiThread, std::ref(toSendQueue), std::ref(incomingQueue), username);
    std::jthread t2(writeThread, std::ref(toSendQueue), std::ref(stream), username);
    std::jthread t3(readThread, std::ref(incomingQueue), std::ref(stream));

    return 0;
}

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatMessage>& incomingQueue, std::string username)
{
    const std::string timeFormat {"[%Y-%m-%d %H:%M] "};
    username = "[" + username + "]: ";
    
    SimpleChatUI UI;
    std::string input;
    ChatMessage incomingMsg;

	while (true)
	{
		UI.handleInput();

        input = UI.takeInput();
        if (!input.empty())
        {
            std::lock_guard<std::mutex> lg(toSendMutex);
            toSendQueue.push(input);
            writeCondVar.notify_one();

            UI.addMessage(getTimestampFormatted(timeFormat) + username + input);
        }

        incomingMutex.lock();
        if (!incomingQueue.empty())
        {
            incomingMsg = incomingQueue.front();

            std::string formattedMsg {
                convertProtobufTime(incomingMsg.sent_at(), timeFormat) 
                + "[" + incomingMsg.username() + "]: " 
                + incomingMsg.text()
            };

            UI.addMessage(formattedMsg);
            incomingQueue.pop();
        }
        incomingMutex.unlock();
	}
}

void writeThread(std::queue<std::string>& toSendQueue, clientReaderWriter& stream, const std::string_view username)
{
    while (true)
    {
        std::unique_lock<std::mutex> ul(toSendMutex);
        writeCondVar.wait(ul, [&]{ return !toSendQueue.empty(); });

        ChatMessage chatMessage;
        google::protobuf::Timestamp* ts = chatMessage.mutable_sent_at();
        ts->set_seconds(std::time(nullptr));

        chatMessage.set_username(username);
        chatMessage.set_text(toSendQueue.front());
        
        toSendQueue.pop();

        stream->Write(chatMessage);
    }
}

void readThread(std::queue<ChatMessage>& incomingQueue, clientReaderWriter& stream)
{
    while (true)
    {
        ChatMessage tempMsg;
        stream->Read(&tempMsg);

        if (!tempMsg.username().empty())
        {
            incomingMutex.lock();
            incomingQueue.push(tempMsg);
            incomingMutex.unlock();
        }
    }

    return;
}