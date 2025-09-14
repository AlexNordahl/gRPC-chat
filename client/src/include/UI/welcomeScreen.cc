#include "welcomeScreen.h"
#include <algorithm>
#include <cctype>
#include <cassert>

WelcomeScreen::WelcomeScreen()
    : BaseUI()
{
    resizeWindow();
}

WelcomeScreen::~WelcomeScreen()
{
    delwin(win_input);
}

void WelcomeScreen::start()
{
    while (true)
    {
        int ch {wgetch(win_input)};

        if (ch == KEY_RESIZE) 
        {
            resizeterm(0, 0);
            clear();
            refresh();
            resizeWindow();
            continue;
        }

        if (ch == ASCII_ESCAPE)
        {
            input.clear();
            break;
        }
        else if (ch == ASCII_BACKSPACE or ch == KEY_BACKSPACE or ch == ASCII_DELETE)
        {
            if (!input.empty())
                input.pop_back();
        }
        else if (ch == ASCII_NEW_LINE)
        {
            if (input.size() < usernameSizeMin)
                continue;

            if (!input.empty())
                break;
        }
        else if (isprint(ch))
        {
            if (input.size() < usernameSizeLimit and std::isalnum(ch))
            {
                input.push_back(static_cast<char>(ch));
            }
        }

        draw_input(win_input, prompt, input);
    }
}

void WelcomeScreen::setUsernameSizeBounds(int min, int limit)
{
    assert(min > 0);
    assert(limit > 0);

    usernameSizeMin = min;
    usernameSizeLimit = limit;
}

void WelcomeScreen::draw_input(WINDOW *win, const std::string& prompt, const std::string &content)
{
    werase(win);
    box(win, 0, 0);

    [[maybe_unused]] int h, w;
    getmaxyx(win, h, w);

    std::string view = prompt + content;
    const int inner_w = w - 2;

    if (static_cast<int>(view.size()) > inner_w) 
    {
        view = view.substr(view.size() - inner_w);
    }

    mvwprintw(win, 1, 1, "%s", view.c_str());

    int curx = std::min(1 + static_cast<int>(view.size()), w - 2);
    wmove(win, 1, curx);

    wrefresh(win);
}

void WelcomeScreen::resizeWindow()
{
    clear();
    refresh();

    if (win_input) 
    {
        werase(win_input);
        wrefresh(win_input);
        delwin(win_input);
        win_input = nullptr;
    }

    int H, W;
    getmaxyx(stdscr, H, W);

    const int win_h = 3;
    const int win_w = std::max(30, std::min(W - 2, 60));
    const int starty = std::max(0, (H - win_h) / 2);
    const int startx = std::max(0, (W - win_w) / 2);

    win_input = newwin(win_h, win_w, starty, startx);
    keypad(win_input, TRUE);

    box(win_input, 0, 0);
    wrefresh(win_input);

    draw_input(win_input, prompt, input);
}