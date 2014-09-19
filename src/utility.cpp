#include <cstdlib>
#include <string>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))

#ifndef USE_TERMIOS_FOR_PASSWORD_INPUT
#define USE_TERMIOS_FOR_PASSWORD_INPUT
#endif

#include <termios.h>
#endif

#include "utility.h"
#include "common.h"

namespace library_manager {


void ClearScreen()
{
    std::system("clear");
}

std::string ReadLine(const std::string &promt)
{
    std::cout << promt;
    std::string line;
    if (!std::getline(std::cin, line))
    {
        throw ExitProgram(1);
    }
    return line;
}

UserID ReadUserID(const std::string &promt)
{
    while (true)
    {
        try
        {
            std::string line(ReadLine(promt));
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

std::string ReadPassword(const std::string &promt)
{
#ifdef USE_TERMIOS_FOR_PASSWORD_INPUT
    termios old_flags;
    tcgetattr(STDIN_FILENO, &old_flags);
    termios new_flgs = old_flags;
    new_flgs.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_flgs);

    std::string line(ReadLine(promt));
    std::cout << std::endl;  // echo an endline

    tcsetattr(STDIN_FILENO, TCSANOW, &old_flags);
    return line;
#else
    return ReadLine(promt);
#endif
}

bool YesOrNo(const std::string &promt)
{
    std::string line(ReadLine(promt));
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
