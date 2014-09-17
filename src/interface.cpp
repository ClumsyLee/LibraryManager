#include <iostream>
#include <boost/lexical_cast.hpp>

#include <cppconn/resultset.h>

#include "context.h"
#include "database_proxy.h"
#include "interface.h"

namespace {

std::string ReadLine(const std::string &promt)
{
    std::cout << promt;
    std::string line;
    if (!std::getline(std::cin, line) || std::cin.eof())
    {
        throw library_manager::Context::ExitProgram(1);
    }
    return line;
}

library_manager::UserID ReadUserID(const std::string &promt)
{
    return boost::lexical_cast<library_manager::UserID>(ReadLine(promt));
}

std::string ReadPassword(const std::string &promt)
{
    return ReadLine(promt);
}

void ShowThisCopy(int index, sql::ResultSet *copies)
{
    using std::cout;

    cout.width(2);
    cout << index << ". " << copies->getString("title")
         << "\n  条形码: " << copies->getString("id")
         << "\n  到期时间: " << copies->getString("due_date");
    int request_num = copies->getInt("request_num");
    if (request_num)
    {
        cout << " +" << boost::lexical_cast<std::string>(request_num)
             << " 预约";
    }
    cout << "\n  索书号: " << copies->getString("call_num") << std::endl;
}

void ShowThisBook(int index, sql::ResultSet *books)
{
    using std::cout;

    cout.width(2);
    cout << index << ". " << books->getString("title")
         << "\n    " << books->getString("author")
         << "\n    " << books->getString("imprint") << std::endl;
}


enum Choice { CHAR, INDEX };
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
            if (index >= 1 || index <= max_index)
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
    using std::cout;
    using std::endl;

    if (context->keyword().empty())
    {
        std::string keyword = ReadLine("请输入关键字 (空行以返回主菜单): ");
        if (keyword.empty())
        {
            context->set_state(&Interface::MainMenu);
            return;
        }
        context->set_keyword('%' + keyword + '%');
    }

    int index = 1;
    std::vector<ISBN> book_list;

    auto result = DatabaseProxy::Instance()->Query(context->keyword());
    while (result->next())
    {
        ShowThisBook(index++, result.get());
        book_list.push_back(result->getUInt64("isbn"));
        cout << endl;
    }

    if (index == 1)  // no result
    {
        cout << "找不到符合条件的书籍\n\n";
    }
    else
    {
        cout << "[编号] 查看对应书籍的详细信息\n";
    }
    cout << "[r] 重新查询          [q] 返回主菜单\n";

    int choice;
    if (GetChoice("rq", index - 1, choice) == Choice::CHAR)
    {
        if (choice == 'r')
            return;  // state need not to change
        else
            context->set_state(&Interface::MainMenu);
    }
    else
    {
        context->set_current_book(book_list[choice - 1]);
        context->set_state(&Interface::ShowBook);
    }
}

void Interface::ShowBook(Context *context)
{
    using std::cout;
    using std::endl;

    {  // book info
        auto book_info =
            DatabaseProxy::Instance()->BookInfo(context->current_book());
        if (!book_info->next())
        {
            throw std::runtime_error(
                "Cannot find a book (ISBN: " +
                boost::lexical_cast<std::string>(context->current_book()) +
                ") that should exist");
        }

        cout << "题名: " << book_info->getString("title")
             << "\n作者: " << book_info->getString("author")
             << "\n出版发行: " << book_info->getString("imprint")
             << "\n索书号: " << book_info->getString("call_num");
        if (!book_info->isNull("abstract"))
             cout << "\n内容简介: " << book_info->getString("abstract");
        if (!book_info->isNull("table_of_contents"))
             cout << "\n目录: " << book_info->getString("table_of_contents");
    }
    cout << "\n\n副本信息:\n";
    {  // copy info
        auto copy_info =
            DatabaseProxy::Instance()->CopiesOfBook(context->current_book());

        while (copy_info->next())
        {
            cout << copy_info->getString("id") << "    "
                 << copy_info->getString("status") << "    "
                 << copy_info->getString("due_date");
            int request_num = copy_info->getInt("request_num");
            if (request_num)
            {
             cout << " +" << boost::lexical_cast<std::string>(request_num)
                  << " 预约";
            }
            cout << endl;
        }
    }

    cout << "\n[e] 查询界面          [m] 主菜单\n";
    int choice;
    GetChoice("em", 0, choice);
    if (choice == 'e')
    {
        context->set_state(&Interface::Query);
    }
    else
    {
        context->set_state(&Interface::MainMenu);
    }
    context->set_current_book(ISBN());
}

Interface * Interface::Instance()
{
    static Interface interface;
    return &interface;
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

    int choice;
    if (GetChoice("eq", borrowed_.size() + requested_.size(), choice)
                == Choice::CHAR)
    {
        if (choice == 'e')
        {
            context->set_current_book(ISBN());
            context->set_state(&Interface::Query);
            return;
        }
        else  // choice == 'q'
        {
            context->set_state(&Interface::WelcomeScreen);
            context->set_current_book(ISBN());
            context->set_user_id(UserID());
            return;
        }
    }
    else  // an index
    {
        // book in borrowed
        if (choice <= static_cast<int>(borrowed_.size()))
            context->set_current_book(borrowed_[choice - 1].first);
        else  // book in requested
            context->set_current_book(
                requested_[choice - borrowed_.size() - 1].first);

        context->set_state(&Interface::ShowBook);
        return;
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
        ShowThisCopy(index++, borrowed.get());
        cout << endl;
    }

    if (index == 1)  // no entry
        cout << "(无)\n\n";
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
        ShowThisCopy(index++, requested.get());
        cout << endl;
    }

    if (index == static_cast<int>(borrowed_.size() + 1))
        cout << "(无)\n\n";
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
