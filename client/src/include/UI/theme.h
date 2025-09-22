#ifndef THEME_H
#define THEME_H

#include <ncurses.h>

enum Color
{
    Red = 1,
    Warning,
    Cyan,
    White,
    Green,
    Magenta,
    Yellow,
};

inline void initTheme()
{
    init_pair(Color::Red, COLOR_RED, COLOR_BLACK);
    init_pair(Color::Yellow, COLOR_YELLOW, COLOR_BLACK);
    init_pair(Color::Cyan, COLOR_CYAN, COLOR_BLACK);
    init_pair(Color::White, COLOR_WHITE, COLOR_BLACK);
    init_pair(Color::Green, COLOR_GREEN, COLOR_BLACK);
    init_pair(Color::Magenta, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(Color::Yellow, COLOR_YELLOW, COLOR_BLACK);
}

#endif