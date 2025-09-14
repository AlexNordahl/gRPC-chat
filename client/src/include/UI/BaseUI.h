#ifndef BASE_UI_H
#define BASE_UI_H

#include <ncurses.h>
#include <string>
#include "theme.h"

class BaseUI
{
public:
    BaseUI()
    {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);

        start_color();
        initTheme();

        getmaxyx(stdscr, rows, cols);
    }

    virtual ~BaseUI() { endwin(); }
protected:
    struct StyledMessage
    {
        std::string content {};
        int color {};
    };

    int rows{}, cols{};

    static constexpr int ASCII_ESCAPE {27};
    static constexpr int ASCII_BACKSPACE {8};
    static constexpr int ASCII_DELETE {127};
    static constexpr int ASCII_NEW_LINE {10};

    size_t usernameSizeMin {};
    size_t usernameSizeLimit {};
};

#endif