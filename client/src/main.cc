#include <iostream>
#include <grpcpp/grpcpp.h>
#include <thread>
#include <mutex>
#include <unistd.h>
#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include "simpleChatUI.h"

std::mutex toSendMutex;
std::mutex incomingMutex;

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatMessage>& incomingQueue);
void writeThread(std::queue<std::string>& toSendQueue, std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>>& stream, const std::string& myUsername);
void readThread(std::queue<ChatMessage>& incomingQueue,  std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>>& stream);

int main()
{
    auto channel {grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())};
    auto stub {ChatService::NewStub(channel)};
    
    grpc::ClientContext context;

    std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>> stream {stub->Chat(&context)};

    std::queue<std::string> toSendQueue;
    std::queue<ChatMessage> incomingQueue;

    const std::string testUsernsme {"TestUser #" + std::to_string(getpid())};

    std::thread t1(uiThread, std::ref(toSendQueue), std::ref(incomingQueue));
    std::thread t2(writeThread, std::ref(toSendQueue), std::ref(stream), testUsernsme);
    std::thread t3(readThread, std::ref(incomingQueue), std::ref(stream));

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatMessage>& incomingQueue)
{
    simpleChatUI UI;
    std::string input;

	while (true)
	{
		UI.refresh();

        input = UI.takeInput();
        if (!input.empty())
        {
            toSendMutex.lock();
            toSendQueue.push(input);
            toSendMutex.unlock();
        }

        incomingMutex.lock();
        if (!incomingQueue.empty())
        {
            UI.addMessage(incomingQueue.front().username(), incomingQueue.front().text());
            incomingQueue.pop();
        }
        incomingMutex.unlock();
	}
}

void writeThread(std::queue<std::string>& toSendQueue, std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>>& stream, const std::string& myUsername)
{
    while (true)
    {
        // spin-waiting, poprawic na condition variable
        toSendMutex.lock();
        if (!toSendQueue.empty())
        {
            ChatMessage chatMessage;

            chatMessage.set_username(myUsername);
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