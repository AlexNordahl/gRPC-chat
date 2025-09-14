#ifndef THEME_H
#define THEME_H

#include <ncurses.h>

enum Color
{
    Error = 1,
    Warning,
    Notify,
    White,
};

inline void initTheme()
{
    init_pair(Color::Error, COLOR_RED, COLOR_BLACK);
    init_pair(Color::Warning, COLOR_YELLOW, COLOR_BLACK);
    init_pair(Color::Notify, COLOR_CYAN, COLOR_BLACK);
    init_pair(Color::White, COLOR_WHITE, COLOR_BLACK);
}

#endif