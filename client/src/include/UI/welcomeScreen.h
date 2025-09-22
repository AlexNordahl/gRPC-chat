#ifndef WELCOME_SCREEN_H
#define WELCOME_SCREEN_H

#include <ncurses.h>
#include <string>
#include "baseUI.h"

class WelcomeScreen : public BaseUI
{
public:
    WelcomeScreen();
    ~WelcomeScreen();

    void start();
    const std::string& username() const { return input; }

private:
    void draw_input(WINDOW *win, const std::string &prompt, const std::string &content);
    void resizeWindow();
    
    WINDOW* win_input {nullptr};

    std::string input {};
    std::string prompt {"Please enter nick: "};

    size_t usernameSizeMin {4};
    size_t usernameSizeLimit {15};
};

#endif