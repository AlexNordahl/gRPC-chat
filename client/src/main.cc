#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include "include/Workers/workers.h"
#include <grpcpp/grpcpp.h>

int main()
{
    auto channel {grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())};
    auto stub {ChatService::NewStub(channel)};

    grpc::ClientContext context {};
    clientReaderWriter stream {stub->Chat(&context)};

    std::queue<std::string> toSendQueue {};
    std::queue<ChatEvent> incomingQueue {};

    WelcomeScreen ws {};
    ws.start();
    std::string username {ws.username()};   

    std::jthread t1(uiThread, std::ref(toSendQueue), std::ref(incomingQueue), username);
    std::jthread t2(writeThread, std::ref(toSendQueue), std::ref(stream), username);
    std::jthread t3(readThread, std::ref(incomingQueue), std::ref(stream));
}