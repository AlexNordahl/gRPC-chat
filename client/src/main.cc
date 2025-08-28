#include <iostream>
#include <grpcpp/grpcpp.h>
#include <thread>
#include <mutex>
#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include "simpleChatUI.h"

std::mutex mtx;

void uiThread(std::queue<std::string>& messageQueue)
{
    simpleChatUI UI;

	while (true)
	{
		UI.refresh();

        mtx.lock();
        std::string input = UI.takeInput();
        if (!input.empty())
            messageQueue.push(input);
        mtx.unlock();
	}
}

int main()
{
    // TODO:
    // dodac testy
    // rodzielic Read i Write na osobne watki

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = ChatService::NewStub(channel);
    
    grpc::ClientContext context;

    auto stream = stub->Chat(&context);

    std::queue<std::string> messageQueue;

    std::thread t1(uiThread, std::ref(messageQueue));

    while (true)
    {
        // spin-waiting, poprawic na condition variable
        mtx.lock();
        if (!messageQueue.empty())
        {
            ChatMessage chatMessage;

            chatMessage.set_username("Szymon");
            chatMessage.set_text(messageQueue.front());
            messageQueue.pop();

            stream->Write(chatMessage);
        }
        mtx.unlock();
    }

    t1.join();

    return 0;
}