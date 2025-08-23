#ifndef SIMPLE_CHAT_UI_H
#define SIMPLE_CHAT_UI_H

#include <ncurses.h>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

class simpleChatUI
{
public:
    simpleChatUI();
    ~simpleChatUI();

    void refresh();
    void addMessage(const std::string& author, const std::string& content);
    std::string takeInput();
    
private:
    static constexpr int ASCII_ESCAPE {27};
    static constexpr int ASCII_BACKSPACE {8};
    static constexpr int ASCII_DELETE {127};
    static constexpr int ASCII_NEW_LINE {10};

    int rows, cols;
    int input_window_height = 3;

    WINDOW* win_msgs = nullptr;
    WINDOW* win_input = nullptr;

    std::vector<std::string> chat;
    std::queue<std::string> msgQueue;
    std::string input;
    const std::string prompt = "> ";

    void draw_messages(WINDOW* win, const std::vector<std::string>& msgs);
    void draw_input(WINDOW* win, const std::string& prompt, const std::string& content);
    void resizeWindow();
};

#endif