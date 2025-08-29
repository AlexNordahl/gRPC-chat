#include "simpleChatUI.h"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>

void simpleChatUI::refresh()
{
    draw_messages(win_msgs, chat);

    resizeWindow();

    int ch = wgetch(win_input);
    if (ch == ASCII_ESCAPE) 
    {
        return;
    }
    else if (ch == ASCII_BACKSPACE or ch == ASCII_DELETE) 
    {
        if (!input.empty())
            input.pop_back();

        draw_input(win_input, prompt, input);
    } 
    else if (ch == ASCII_NEW_LINE) 
    {
        if (!input.empty())
        {
            chat.push_back(getTimestampFormatted("[%Y-%m-%d %H:%M]") + myUsername + input);
            msgQueue.push(input);
            input.clear();
            draw_messages(win_msgs, chat);
        }
        draw_input(win_input, prompt, input);
    } 
    else if (isprint(ch))
    {
        input.push_back(static_cast<char>(ch));
        draw_input(win_input, prompt, input);
    }
}

void simpleChatUI::addMessage(const std::string &message)
{
    chat.push_back(message);
}

std::string simpleChatUI::takeInput()
{
    if (msgQueue.empty())
        return "";
    
    std::string msg = msgQueue.front();
    msgQueue.pop();
    return msg;
}

simpleChatUI::simpleChatUI(std::string myUsername, std::string prompt)
 : myUsername(myUsername), prompt(prompt) 
{
    initscr();
    curs_set(0);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr, rows, cols);

    win_msgs  = newwin(rows - input_window_height, cols, 0, 0);
    win_input = newwin(input_window_height, cols, rows - input_window_height, 0);
    
    scrollok(win_msgs, TRUE);
    wtimeout(win_input, 100);

    draw_messages(win_msgs, chat);
    draw_input(win_input, prompt, input);
}

simpleChatUI::~simpleChatUI()
{
    delwin(win_input);
    delwin(win_msgs);
    endwin();
}

void simpleChatUI::draw_messages(WINDOW *win, const std::vector<std::string> &msgs)
{
    werase(win);
    box(win, 0, 0);
    int h, w; getmaxyx(win, h, w);
    int max_lines = h - 2;
    int start = (int)msgs.size() > max_lines ? (int)msgs.size() - max_lines : 0;
    int y = 1;

    for (int i = start; i < msgs.size(); ++i, ++y) 
	{
        std::string line = msgs[i];

        if ((int)line.size() > w - 2)
            line.resize(w - 2);

        mvwprintw(win, y, 1, "%s", line.c_str());
    }

    wrefresh(win);
}

void simpleChatUI::draw_input(WINDOW *win, const std::string &prompt, const std::string &content)
{
    werase(win);
    box(win, 0, 0);
    [[maybe_unused]] int h, w;
    getmaxyx(win, h, w);
    std::string view = prompt + content;

    if (view.size() > w - 2) 
	{
        view = view.substr(view.size() - (w - 2));
    }
	
    mvwprintw(win, 1, 1, "%s", view.c_str());
    int curx = 1 + (int)view.size();

    if (curx >= w - 1)
        curx = w - 2;

    wmove(win, 1, curx);
    wrefresh(win);
}

void simpleChatUI::resizeWindow()
{
    int r, c; getmaxyx(stdscr, r, c);
    if (r != rows or c != cols) 
    {
        rows = r; cols = c;
        wclear(stdscr); refresh();
        wresize(win_msgs, rows - input_window_height, cols);
        wresize(win_input, input_window_height, cols);
        mvwin(win_input, rows - input_window_height, 0);
        draw_messages(win_msgs, chat);
        draw_input(win_input, prompt, input);
    }
}

std::string simpleChatUI::getTimestampFormatted(const std::string &format)
{
    time_t t = static_cast<time_t>(std::time(nullptr));
    std::tm tm{};
    localtime_r(&t, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, format.c_str());

    return oss.str();
}