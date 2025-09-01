#include <iostream>
#include <grpcpp/grpcpp.h>
#include <thread>
#include <mutex>
#include <unistd.h>
#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include "include/simpleChatUI.h"
#include "include/timeFunctions.h"
#include "welcomeScreen.h"

std::mutex toSendMutex;
std::mutex incomingMutex;

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatMessage>& incomingQueue);
void writeThread(std::queue<std::string>& toSendQueue, std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>>& stream, const std::string_view myUsername);
void readThread(std::queue<ChatMessage>& incomingQueue,  std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>>& stream);

int main()
{
    auto channel {grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())};
    auto stub {ChatService::NewStub(channel)};
    grpc::ClientContext context;
    std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>> stream {stub->Chat(&context)};

    std::queue<std::string> toSendQueue;
    std::queue<ChatMessage> incomingQueue;

    std::string username {};
    {
        welcomeScreen ws;
        ws.start();
        if (!ws.nick().empty())
        username = ws.nick();
    }

    std::jthread t1(uiThread, std::ref(toSendQueue), std::ref(incomingQueue));
    std::jthread t2(writeThread, std::ref(toSendQueue), std::ref(stream), username);
    std::jthread t3(readThread, std::ref(incomingQueue), std::ref(stream));
    
    return 0;
}

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatMessage>& incomingQueue)
{
    const std::string myUsername {"[You]: "};
    const std::string timeFormat {"[%Y-%m-%d %H:%M] "};
    simpleChatUI UI;
    std::string input;
    ChatMessage incomingMsg;

	while (true)
	{
		UI.handleInput();

        input = UI.takeInput();
        if (!input.empty())
        {
            toSendMutex.lock(); 
            toSendQueue.push(input);
            toSendMutex.unlock();

            UI.addMessage(getTimestampFormatted(timeFormat) + myUsername + input);
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

void writeThread(std::queue<std::string>& toSendQueue, std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>>& stream, const std::string_view username)
{
    ChatMessage chatMessage;

    while (true)
    {
        // spin-waiting, poprawic na condition variable
        toSendMutex.lock();
        if (!toSendQueue.empty())
        {
            google::protobuf::Timestamp* ts = chatMessage.mutable_sent_at();
            ts->set_seconds(std::time(nullptr));

            chatMessage.set_username(username);
            chatMessage.set_text(toSendQueue.front());
            
            toSendQueue.pop();

            stream->Write(chatMessage);
        }
        toSendMutex.unlock();
    }
}

void readThread(std::queue<ChatMessage>& incomingQueue,  std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>>& stream)
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