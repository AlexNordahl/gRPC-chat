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
    void UpdateUsers(const std::vector<std::string>& usernames);
    std::string takeInput();
    
private:
    int input_window_height {3};
    int sidebar_width {24};

    WINDOW* win_msgs {nullptr};
    WINDOW* win_input {nullptr};
    WINDOW* win_side  {nullptr};

    std::vector<StyledMessage> chat {};
    std::queue<std::string> msgQueue {};
    std::vector<std::string> users {};
    std::string input {};
    
    const std::string prompt {"> "};

    void draw_messages(WINDOW* win, const std::vector<StyledMessage>& msgs);
    void draw_input(WINDOW *win, const std::string_view prompt, const std::string_view content);
    void draw_sidebar(WINDOW* win, const std::vector<std::string>& users);
    void resizeWindow();
};

#endif