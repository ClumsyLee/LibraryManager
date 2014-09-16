#include <iostream>
#include <boost/lexical_cast.hpp>

#include <cppconn/resultset.h>

#include "context.h"
#include "database_proxy.h"
#include "interface.h"

namespace {

void ShowThisBook(int index, sql::ResultSet *books)
{
    using std::cout;

    cout.width(2);
    cout << index << ". " << books->getString("title")
         << "\n  条形码: " << books->getString("id")
         << "\n  到期时间: " << books->getString("due_date");
    int request_num = books->getInt("request_num");
    if (request_num)
    {
        cout << " +" << boost::lexical_cast<std::string>(request_num)
             << " 预约";
    }
    cout << "\n  索书号: " << books->getString("call_num") << std::endl;
}


}  // namespace

namespace library_manager {

void Interface::WelcomeScreen(Context *context)
{
    // TODO: set different mode for reader login / admin login / lookup
    std::cout << "欢迎来到图书馆管理系统\n";

    UserID user_id;
    User user;
    GetValidUser(user_id, user);

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
        default:
        {
            throw std::runtime_error(
                "Bad return value from GetValidUser: " +
                boost::lexical_cast<std::string>(static_cast<int>(user)));
        }
    }
    // Login successful, entering main menu.
    context->set_state(&Interface::MainMenu);
    context->set_user_id(user_id);
    std::cout << "登陆成功\n";
}

void Interface::Query(Context *context)
{

}

Interface * Interface::Instance()
{
    static Interface interface;
    return &interface;
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

void Interface::GetValidUser(UserID &user_id, User &user)
{
    using std::cout;

    while (true)
    {
        UserID id = ReadUserID("请输入用户ID: ");
        std::string password = ReadPassword("请输入密码: ");
        if (DatabaseProxy::Instance()->Login(id, password, user))
        {
            user_id = id;
            return;
        }
        // Login failed.
        cout << "用户名或密码错误\n";
    }
}

void ReaderInterface::MainMenu(Context *context)
{
    using std::cout;
    using std::endl;

    user_id_ = context->user_id();  // update user_id

    ShowReaderInfo();
    ShowBorrowed();
    ShowRequested();

    cout << "[编号] 查看对应书籍的详细信息\n"
            "[e] 进入查询界面         [q] 退出\n";
    while (true)
    {
        std::string line = ReadLine(": ");
        if (line == "e")
        {
            current_book_ = ISBN();
            context->set_state(&Interface::ShowBook);
            return;
        }
        else if (line == "q")
        {
            context->set_state(&Interface::WelcomeScreen);
            return;
        }
        else  // may be an index
        {
            try
            {
                int index = boost::lexical_cast<int>(line);
                if (index < 1 || index > (borrowed_.size() + requested_.size()))
                {
                    cout << "无效的编号\n";
                }
                else
                {
                    if (index <= borrowed_.size())  // book in borrowed
                        current_book_ = borrowed_[index - 1].first;
                    else  // book in requested
                        current_book_ =
                            requested_[index - borrowed_.size() - 1].first;

                    context->set_state(&Interface::Query);
                    return;
                }
            }
            catch (const boost::bad_lexical_cast &)
            {
                cout << "无效的输入\n";
            }
        }
        // failed to choose, get line again
    }
}

ReaderInterface * ReaderInterface::Instance()
{
    static ReaderInterface interface;
    return &interface;
}

void ReaderInterface::ShowReaderInfo() const
{
    using std::cout;

    auto user_info = DatabaseProxy::Instance()->ReaderInfo(user_id_);
    if (!user_info->next())
        throw std::runtime_error("Cannot find a user (id: " +
            boost::lexical_cast<std::string>(user_id_) +
            ") that has already logged in");

    cout << "姓名: " << user_info->getString("name")
         << "\n\n";
}

void ReaderInterface::ShowBorrowed()
{
    using std::cout;
    using std::endl;
    // clear previous borrowed info
    borrowed_.clear();

    auto borrowed = DatabaseProxy::Instance()->QueryBorrowed(user_id_);
    int index = 1;

    cout << "所借书籍:\n";
    while (borrowed->next())
    {
        borrowed_.emplace_back(borrowed->getUInt64("isbn"),
                               borrowed->getString("id"));
        ShowThisBook(index++, borrowed.get());
        cout << endl;
    }

    if (index == 1)  // no entry
        cout << "(无)\n";
    cout << endl;
}

void ReaderInterface::ShowRequested()
{
    using std::cout;
    using std::endl;
    // clear previous requested info
    requested_.clear();

    auto requested = DatabaseProxy::Instance()->QueryRequested(user_id_);
    int index = borrowed_.size() + 1;

    cout << "预约书籍:\n";
    while (requested->next())
    {
        requested_.emplace_back(requested->getUInt64("isbn"),
                                requested->getString("id"));
        ShowThisBook(index++, requested.get());
    }

    if (index == borrowed_.size() + 1)
        cout << "(无)\n";
    cout << endl;
}



// AdminInterface
void AdminInterface::MainMenu(Context *context)
{
}

AdminInterface * AdminInterface::Instance()
{
    static AdminInterface interface;
    return &interface;
}


}  // namespace library_manager
