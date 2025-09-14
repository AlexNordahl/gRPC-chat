#include "proto/chat.grpc.pb.h"
#include "proto/chat.pb.h"
#include "include/UI/simpleChatUI.h"
#include "include/UI/welcomeScreen.h"
#include "include/Utility/timeFunctions.h"
#include "include/UI/theme.h"
#include <grpcpp/grpcpp.h>
#include <thread>
#include <mutex>
#include <condition_variable>

using clientReaderWriter = std::shared_ptr<grpc::ClientReaderWriter<ChatEvent, ChatEvent>>;

std::condition_variable writeCondVar;
std::mutex toSendMutex;
std::mutex incomingMutex;

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatEvent>& incomingQueue, std::string username);
void writeThread(std::queue<std::string>& toSendQueue, clientReaderWriter& stream, const std::string_view myUsername);
void readThread(std::queue<ChatEvent>& incomingQueue, clientReaderWriter& stream);

int main()
{
    auto channel {grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())};
    auto stub {ChatService::NewStub(channel)};
    grpc::ClientContext context {};
    clientReaderWriter stream {stub->Chat(&context)};

    std::queue<std::string> toSendQueue {};
    std::queue<ChatEvent> incomingQueue {};

    std::string username {};

    {
        WelcomeScreen ws {};
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

void uiThread(std::queue<std::string>& toSendQueue, std::queue<ChatEvent>& incomingQueue, std::string username)
{
    SimpleChatUI UI {};
    std::string input {};
    ChatEvent incomingMsg {};

	while (true)
	{
		UI.handleInput();

        input = UI.takeInput();
        if (!input.empty())
        {
            std::lock_guard<std::mutex> lg(toSendMutex);
            toSendQueue.push(input);
            writeCondVar.notify_one();

            UI.addMessage(getTimestamp() + "[" + username + "]: " + input, Color::White);
        }

        std::lock_guard<std::mutex> lock(incomingMutex);
        if (!incomingQueue.empty())
        {
            incomingMsg = incomingQueue.front();
            ChatMessage msg;
            std::string formattedMsg;
            
            switch (incomingMsg.payload_case())
            {
            case ChatEvent::kChatMessage:
                msg = incomingMsg.chat_message();
                UI.addMessage(convertProtoTime(msg.sent_at()) + "[" + msg.username() + "]: " + msg.text(), msg.color());
                incomingQueue.pop();
                break;
            case ChatEvent::kUserJoined:
                // to do
                break;
            case ChatEvent::kUserLeft:
                // to do
                break;
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

        ChatEvent ev {};
        ChatMessage* msg {ev.mutable_chat_message()};
        google::protobuf::Timestamp* ts = msg->mutable_sent_at();
        ts->set_seconds(std::time(nullptr));

        msg->set_username(username);
        msg->set_text(toSendQueue.front());
        msg->set_color(Color::White);
        
        toSendQueue.pop();
        ul.unlock(); 

        stream->Write(ev);
    }
}

void readThread(std::queue<ChatEvent>& incomingQueue, clientReaderWriter& stream)
{
    while (true)
    {
        ChatEvent ev {};
        stream->Read(&ev);
        std::lock_guard<std::mutex> lock(incomingMutex);
        incomingQueue.push(ev);
    }

    return;
}