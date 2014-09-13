#include <iostream>
#include <boost/lexical_cast.hpp>

#include <cppconn/resultset.h>

#include "common.h"
#include "context.h"
#include "database_proxy.h"
#include "interface.h"

namespace library_manager {

void Interface::WelcomeScreen(Context *context)
{
    // TODO: set different mode for reader login / admin login / lookup
    std::cout << "欢迎来到图书馆管理系统\n";
    User user = GetValidUser();

    switch (user)
    {
        case User::READER:
        {
            context->set_interface(ReaderInterface::Instance());
            break;
        }
        case User::ADMIN:
        {
            context->set_interface(AdminInterface::Instance());
            break;
        }
        case User::GUEST:
        {
            context->set_interface(GuestInterface::Instance());
            break;
        }
        default:
        {
            throw std::runtime_error(
                "Bad return value from GetValidUser: " +
                boost::lexical_cast<std::string>(user));
        }
    }
    // Login successful, entering main menu.
    context->set_state(&Interface::MainMenu);
}

std::string Interface::ReadLine(const std::string &promt)
{
    std::cout << promt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

UserID Interface::ReadUserID(const std::string &promt)
{
    return boost::lexical_cast<UserID>(ReadLine(promt));
}


std::string Interface::ReadPassword(const std::string &promt)
{
    return ReadLine(promt);
}

User Interface::GetValidUser()
{
    using std::cout;

    while (true)
    {
        UserID user_id = ReadUserID("请输入用户ID: ");
        std::string password = ReadPassword("请输入密码: ");
        User user = DatabaseProxy::Instance()->Login(user_id, password);
        if (user != User::NONE)
            return user;
        // Login failed.
        cout << "用户名或密码错误\n";
    }
}

void ReaderInterface::MainMenu(Context *context)
{
    ShowReaderInfo();
    ShowBorrowed();
    ShowRequested();
}

void ReaderInterface::ShowReaderInfo() const
{
    using std::cout;

    auto user_info = DatabaseProxy::Instance()->ReaderInfo(user_id());
    if (!user_info->next())
        throw std::runtime_error("Cannot find a user (id: " +
            boost::lexical_cast<std::string>(user_id()) +
            ") that has already logged in");

    cout << "姓名: " << user_info->getString("name")
         << "\n电话: " << user_info->getUInt64("phone_num")
         << "\n\n";
}

void ReaderInterface::ShowBorrowed()
{
    using std::cout;

    // clear previous borrowed info
    borrowed_.clear();

    auto borrowed = DatabaseProxy::Instance()->QueryBorrowed(user_id());
    int index = 1;

    cout << "所借书籍:\n";
    while (borrowed->next())
    {
        borrowed_.push_back(borrowed->getUInt64("isbn"));
        cout << index++ << ": "
             << borrowed->getString("title") << "  "
             << 
    }
}

void ReaderInterface::ShowRequested()
{

}

// AdminInterface
void AdminInterface::MainMenu(Context *context)
{
}



}  // namespace library_manager
