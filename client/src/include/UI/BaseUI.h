#ifndef BASE_UI_H
#define BASE_UI_H

#include <ncurses.h>
#include <string>

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
        init_pair(1, COLOR_BLACK, COLOR_BLUE);
        init_pair(2, COLOR_WHITE, COLOR_BLUE);
        bkgd(COLOR_PAIR(1)); clear(); refresh();

        getmaxyx(stdscr, rows, cols);
    }

    virtual ~BaseUI() { endwin(); }
protected:
    int rows{}, cols{};

    static constexpr int ASCII_ESCAPE {27};
    static constexpr int ASCII_BACKSPACE {8};
    static constexpr int ASCII_DELETE {127};
    static constexpr int ASCII_NEW_LINE {10};

    size_t usernameSizeMin {};
    size_t usernameSizeLimit {};
};

#endif