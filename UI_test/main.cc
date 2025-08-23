#include "simpleChatUI.hh"
#include <thread>
#include <mutex>
#include <queue>

std::mutex mtx;
std::queue<std::string> messages;

void queueMessage()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(3));

		mtx.lock();
		messages.push("Hi");
		mtx.unlock();
	}
}

int main(int argc, char const *argv[])
{
	simpleChatUI UI;

	std::string author {"Szymon"};

	std::thread t1(queueMessage);

	while (true)
	{
		if (!messages.empty())
		{
			mtx.lock();
			UI.addMessage(author, messages.front());
			messages.pop();
			mtx.unlock();
		}

		UI.refresh();
	}

	t1.join();

	return 0;
}