#include <cstdlib>
#include <string>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))

#ifndef UNIX_LIKE_SYS
#define UNIX_LIKE_SYS
#endif

#include <readline/readline.h>
#include <readline/history.h>

#include <termios.h>

#elif defined(_WIN32)

#include <windows.h>

#endif

#include "utility.h"
#include "common.h"

namespace library_manager {


void ClearScreen()
{
#if defined(_WIN32)
    std::system("cls");
#elif defined(UNIX_LIKE_SYS)
    std::system("clear");
#endif
}

std::string ReadLine(const std::string &prompt)
{
    std::string line;

#ifdef UNIX_LIKE_SYS
    char *line_read = readline(prompt.c_str());
    if (!line_read)  // EOF
        throw ExitProgram(1);

    line.assign(line_read);

    if (!boost::trim_copy(line).empty())
        add_history(line_read);  // if not empty, add the origin line

    std::free(line_read);
#else
    std::cout << prompt;
    std::getline(std::cin, line);
#endif

    return line;
}

UserID ReadUserID(const std::string &prompt)
{
    while (true)
    {
        try
        {
            std::string line(ReadLine(prompt));
            if (line.empty())
                return kInvalidUserID;  // return if enter an empty user id

            return boost::lexical_cast<UserID>(line);
        }
        catch (boost::bad_lexical_cast &)
        {
            std::cout << "ID格式错误, 注意ID为纯数字\n";
        }
    }
}

std::string ReadPassword(const std::string &prompt)
{
    std::cout << prompt;
    std::string line;

#ifdef UNIX_LIKE_SYS
    termios old_flags;
    tcgetattr(STDIN_FILENO, &old_flags);
    termios new_flgs = old_flags;
    new_flgs.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_flgs);

    std::getline(std::cin, line);
    std::cout << std::endl;  // echo an endline

    tcsetattr(STDIN_FILENO, TCSANOW, &old_flags);
#elif defined(_WIN32)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

    std::getline(std::cin, line);
    std::cout << std::endl;  // echo an endline
    SetConsoleMode(hStdin, mode);
#endif

    return line;
}

bool YesOrNo(const std::string &prompt)
{
    std::string line(ReadLine(prompt));
    while (true)
    {
        boost::to_lower(line);
        if (line == "y" || line == "yes")
            return true;
        if (line == "n" || line == "no")
            return false;
        line.assign(ReadLine("请输入 y/n: "));
    }
}

Choice GetChoice(const std::string &chars, int max_index, int &choice)
{
    using std::cout;

    while (true)
    {
        std::string line = ReadLine(":");
        if (line.size() == 1)
        {
            char ch = line[0];
            if (chars.find(ch) != chars.npos)
            {
                choice = ch;
                return Choice::CHAR;
            }
        }
        try  // try to treat it as an index
        {
            int index = boost::lexical_cast<int>(line);
            if (index >= 1 && index <= max_index)
            {
                choice = index;
                return Choice::INDEX;
            }
        }
        catch (const boost::bad_lexical_cast &)
        {
        }
        // failed to get choich, read line again
        cout << "无效的输入\n";
    }
}

int GetChoice(const std::string &chars)
{
    int choice;
    GetChoice(chars, 0, choice);
    return choice;
}

int GetChoice(int max_index)
{
    int choice;
    GetChoice("", max_index, choice);
    return choice;
}

}  // namespace library_manager
