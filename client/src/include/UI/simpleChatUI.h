#ifndef SIMPLE_CHAT_UI_H
#define SIMPLE_CHAT_UI_H

#include <ncurses.h>
#include <vector>
#include <queue>
#include <string>
#include "BaseUI.h"

class SimpleChatUI : public BaseUI
{
public:
    SimpleChatUI();
    ~SimpleChatUI();

    void handleInput();
    void addMessage(const std::string_view message, int color);
    std::string takeInput();
    
private:
    int input_window_height {3};

    WINDOW* win_msgs {nullptr};
    WINDOW* win_input {nullptr};

    std::vector<StyledMessage> chat {};
    std::queue<std::string> msgQueue {};
    std::string input {};
    
    const std::string prompt {"> "};

    void draw_messages(WINDOW* win, const std::vector<StyledMessage>& msgs);
    void draw_input(WINDOW *win, const std::string_view prompt, const std::string_view content);
    void resizeWindow();
};

#endif