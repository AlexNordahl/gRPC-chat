#ifndef SIMPLE_CHAT_UI_H
#define SIMPLE_CHAT_UI_H

#include <ncurses.h>
#include <vector>
#include <string>
#include <chrono>

class simpleChatUI
{
public:
    
    void start();

private:

    int rows, cols;
    int input_window_height = 3;

    WINDOW* win_msgs = nullptr;
    WINDOW* win_input = nullptr;

    std::vector<std::string> chat;
    std::string input;
    const std::string prompt = "> ";

    void draw_messages(WINDOW* win, const std::vector<std::string>& msgs);
    void draw_input(WINDOW* win, const std::string& prompt, const std::string& buf);
};

#endif