#ifndef WELCOME_SCREEN_H
#define WELCOME_SCREEN_H

#include <ncurses.h>
#include <string>

class WelcomeScreen
{
public:
    WelcomeScreen();
    ~WelcomeScreen();

    void start();
    void setUsernameSizeBounds(int min, int limit);
    const std::string& username() const { return input; }

private:
    void draw_input(WINDOW *win, const std::string &prompt, const std::string &content);
    void resizeWindow();

    static constexpr int ASCII_ESCAPE {27};
    static constexpr int ASCII_BACKSPACE {8};
    static constexpr int ASCII_DELETE {127};
    static constexpr int ASCII_NEW_LINE {10};

    size_t usernameSizeMin {5};
    size_t usernameSizeLimit {15};
    
    WINDOW* win_input {nullptr};

    std::string input;
    std::string prompt {"Please enter nick: "};
};

#endif