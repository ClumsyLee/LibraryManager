#include <iostream>
#include <boost/lexical_cast.hpp>

#include <cppconn/resultset.h>

#include "context.h"
#include "database_proxy.h"
#include "interface.h"
#include "utility.h"

namespace {

void ShowThisCopy(int index, sql::ResultSet *copies)
{
    using std::cout;

    cout.width(2);
    cout << index << ". " << copies->getString("title")
         << "\n  条形码: " << copies->getString("id");
    if (!copies->isNull("due_date"))
         cout << "\n  到期时间: " << copies->getString("due_date");
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

bool ShowCopyBasicInfo(const library_manager::CopyID &copy_id)
{
    using std::cout;
    using std::endl;

    auto copy = library_manager::DatabaseProxy::Instance()->CopyInfo(copy_id);
    if (!copy->next())
        return false;

    cout << copy->getString("id") << "    "
         << copy->getString("title") << "    "
         << copy->getString("call_num") << endl;
    return true;
}

}  // namespace

namespace library_manager {

void Interface::WelcomeScreen(Context *context)
{
    // Clear current context
    context->set_interface(Interface::Instance());
    context->set_user_id(UserID());
    context->set_keyword("");
    context->set_current_book(ISBN());

    ClearScreen();
    std::cout <<
" ==============================================================\n"
"|                    欢迎来到图书馆管理系统                    |\n"
"|                             v1.0                             |\n"
" ==============================================================\n"
"\n"
"[l] 登陆           [e] 查询书籍       [r] 还书           [q] 退出\n";

    int choice = GetChoice("lerq");

    if (choice == 'l')
        context->set_state(&Interface::Login);
    else if (choice == 'e')
        context->set_state(&Interface::Query);
    else if (choice == 'r')
        context->set_state(&Interface::ReturnBook);
    else  // quit
        throw ExitProgram(0);
}

void Interface::Login(Context *context)
{
    using std::cout;

    ClearScreen();
    cout << " ============================ 登陆 ============================\n";

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
        case User::ADMIN_READER:
        {
            context->set_interface(AdminReaderInterface::Instance());
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
    cout << "登陆成功\n";
}

void Interface::MainMenu(Context *context)
{
    context->set_state(&Interface::WelcomeScreen);
}

void Interface::BorrowBook(Context *context)
{
    context->set_state(&Interface::WelcomeScreen);
}

void Interface::RequestBook(Context *context)
{
    context->set_state(&Interface::WelcomeScreen);
}

void Interface::ShowUser(Context *context)
{
    context->set_state(&Interface::WelcomeScreen);
}

void Interface::CreateUser(Context *context)
{
    context->set_state(&Interface::WelcomeScreen);
}

void Interface::DeleteUser(Context *context)
{
    context->set_state(&Interface::WelcomeScreen);
}

void Interface::ChangePassword(Context *context)
{
    using std::cout;
    using std::endl;

    DatabaseProxy * proxy = DatabaseProxy::Instance();

    ClearScreen();
    cout << " ======================== 修改密码 ========================\n";

    cout << "ID: " << context->user_id() << endl;

    std::string old_password(ReadPassword("原密码: "));
    User user;
    while (!proxy->Login(context->user_id(), old_password, user))
    {
        if (old_password.empty())
        {
            context->set_state(&Interface::MainMenu);
            return;
        }
        old_password.assign(ReadPassword("密码错误\n原密码: "));
    }

    std::string new_password(ReadPassword("新密码: "));
    if (new_password == ReadPassword("确认密码: "))
    {
        if (proxy->ChangePassword(context->user_id(),
                                  old_password, new_password))
            cout << "修改成功\n";
        else
            cout << "修改失败\n";
    }
    else
    {
        cout << "两次输入的密码不同\n";
    }

    ContinueOrMainMenu(context);
}

void Interface::ReturnBook(Context *context)
{
    using std::cout;
    using std::endl;

    ClearScreen();
    cout << " ========================== 还书 ==========================\n";

    CopyID copy_id = ReadLine("请输入要归还书籍的条形码: ");
    if (copy_id.empty())  // return if got an empty line
    {
        context->set_state(&Interface::MainMenu);
        return;
    }

    if (DatabaseProxy::Instance()->ReturnCopy(copy_id))
    {
        ShowCopyBasicInfo(copy_id);
        cout << "归还成功\n";
    }
    else
    {
        cout << "归还失败\n";
    }

    ContinueOrMainMenu(context);
}

void Interface::GetRequested(Context *context)
{
    context->set_state(&Interface::WelcomeScreen);
}

void Interface::Query(Context *context)
{
    using std::cout;
    using std::endl;

    ClearScreen();
    cout << " ========================== 查询书籍 ==========================\n";

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
    cout << "[r] 重新查询                              [m] 主菜单\n";

    int choice;
    if (GetChoice("rm", index - 1, choice) == Choice::CHAR)
    {
        if (choice == 'r')
        {
            context->set_keyword("");
            return;  // state need not to change
        }
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

    ClearScreen();
    cout << " ========================== 书籍信息 ==========================\n";

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
                 << copy_info->getString("status") << "    ";
            if (!copy_info->isNull("due_date"))
                 cout << copy_info->getString("due_date");
            int request_num = copy_info->getInt("request_num");
            if (request_num)
            {
             cout << " +" << boost::lexical_cast<std::string>(request_num)
                  << " 预约";
            }
            cout << endl;
        }
    }

    cout << "\n[e] 查询界面                              [m] 主菜单\n";
    int choice = GetChoice("em");
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

void Interface::ContinueOrMainMenu(Context *context)
{
    std::cout << "\n[c] 继续                              [m] 主菜单\n";
    int choice = GetChoice("cm");
    if (choice == 'm')
        context->set_state(&Interface::MainMenu);
    // else choice == 'c', nothing needs to be done
}

std::string Interface::GetUserName(Context *context)
{
    auto result = DatabaseProxy::Instance()->ReaderInfo(context->user_id());
    if (!result->next())
        throw std::runtime_error("Cannot find a user (id: " +
            boost::lexical_cast<std::string>(context->user_id()) +
            ") that has already logged in");

    return result->getString("name");
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
        cout << "用户ID或密码错误\n";
    }
}

void ReaderInterface::MainMenu(Context *context)
{
    using std::cout;
    using std::endl;

    ClearScreen();
    std::cout <<
" =========================== 主菜单 ===========================\n"
"欢迎你, 读者 " << GetUserName(context) << endl;

    PerpareMainMenuAndShowInfo(context);

    cout <<
"[编号] 查看对应书籍的详细信息\n"
"[e] 查询书籍\n"
"[b] 借书           [t] 还书           [r] 预约           [g] 预约取书\n"
"[p] 修改密码                                             [q] 退出\n";

    int choice;
    if (GetChoice("ebtrgpq", borrowed_.size() + requested_.size(), choice)
                == Choice::CHAR)
    {
        switch (choice)
        {
            case 'e': context->set_state(&Interface::Query); break;
            case 'b': context->set_state(&Interface::BorrowBook); break;
            case 't': context->set_state(&Interface::ReturnBook); break;
            case 'r': context->set_state(&Interface::RequestBook); break;
            case 'g': context->set_state(&Interface::GetRequested); break;
            case 'p': context->set_state(&Interface::ChangePassword); break;
            case 'q': context->set_state(&Interface::WelcomeScreen); break;
        }
    }
    else  // an index
    {
        ShowBookInfoFromMain(context, choice);
        return;
    }
}

void ReaderInterface::BorrowBook(Context *context)
{
    using std::cout;
    using std::endl;

    ClearScreen();
    cout << " ============================ 借书 ============================\n";

    CopyID copy_id = ReadLine("请输入要借出副本的条形码: ");
    if (copy_id.empty())  // return if got an empty line
    {
        context->set_state(&Interface::MainMenu);
        return;
    }

    auto copy_info = DatabaseProxy::Instance()->CopyInfo(copy_id);
    if (!copy_info->next())
    {
        cout << "不存在条形码为 " << copy_id << " 的副本\n";
    }
    else if (copy_info->getString("status") != "ON_SHELF")
    {
        cout << "该副本当前不在架上, 不能借出\n";
    }
    else  // able to borrow
    {
        ShowCopyBasicInfo(copy_id);
        if (YesOrNo("确定要借出该副本吗? (y/n): "))
        {
            if (DatabaseProxy::Instance()->BorrowCopy(context->user_id(),
                                                      copy_id))
                cout << "借阅成功\n";
            else
                cout << "借阅失败\n";
        }
        else
        {
            cout << "取消了借出操作\n";
        }
    }

    ContinueOrMainMenu(context);
}

void ReaderInterface::RequestBook(Context *context)
{
    using std::cout;
    using std::endl;

    ClearScreen();
    cout << " ========================== 预约图书 ==========================\n";

    CopyID copy_id = ReadLine("请输入要预约副本的条形码: ");
    if (copy_id.empty())  // return if got an empty line
    {
        context->set_state(&Interface::MainMenu);
        return;
    }

    auto copy_info = DatabaseProxy::Instance()->CopyInfo(copy_id);
    if (!copy_info->next())
    {
        cout << "不存在条形码为 " << copy_id << " 的副本\n";
    }
    else if (copy_info->getString("status") != "LENT")
    {
        cout << "该副本未被借出, 不能预约\n";
    }
    else  // able to request
    {
        ShowCopyBasicInfo(copy_id);
        if (YesOrNo("确定要预约该副本吗? (y/n): "))
        {
            if (DatabaseProxy::Instance()->RequestCopy(context->user_id(),
                                                       copy_id))
                cout << "预约成功\n";
            else
                cout << "预约失败\n";
        }
        else
        {
            cout << "取消了预约操作\n";
        }
    }

    ContinueOrMainMenu(context);
}

void ReaderInterface::GetRequested(Context *context)
{
    using std::cout;
    using std::endl;

    ClearScreen();
    cout << " ======================== 取回预约图书 ========================\n";

    std::vector<CopyID> request_list;
    int index = 1;

    auto proxy = DatabaseProxy::Instance();
    for (const TempBook &pair : requested_)
    {
        if (proxy->AbleToGetRquested(context->user_id(), pair.second))
            request_list.push_back(pair.second);
    }

    if (request_list.empty())
    {
        cout << "当前没有待取的副本\n";
        context->set_state(&Interface::MainMenu);
    }
    else
    {
        cout << "当前可以取回的副本有:\n\n";
        for (const CopyID &copy_id : request_list)
        {
            auto copy_info = proxy->CopyInfo(copy_id);
            if (!copy_info->next())
                continue;
            ShowThisCopy(index++, copy_info.get());
        }
        cout << "[编号] 取回相应副本           ";
    }

    cout << "[m] 主菜单\n";
    int choice;
    if (GetChoice("m", index - 1, choice) == Choice::INDEX)
    {
        if (proxy->GetRequested(context->user_id(), request_list[choice - 1]))
            cout << "取书成功\n";
        else
            cout << "取书失败\n";
    }
    else
    {
        // choice == 'm'
        context->set_state(&Interface::MainMenu);
        return;
    }

    ContinueOrMainMenu(context);
}

ReaderInterface * ReaderInterface::Instance()
{
    static ReaderInterface interface;
    return &interface;
}

int ReaderInterface::PerpareMainMenuAndShowInfo(Context *context)
{
    user_id_ = context->user_id();  // update user_id

    ShowBorrowed();
    ShowRequested();

    return borrowed_.size() + requested_.size();
}

void ReaderInterface::ShowBookInfoFromMain(Context *context, int choice)
{
    // book in borrowed
    if (choice <= static_cast<int>(borrowed_.size()))
        context->set_current_book(borrowed_[choice - 1].first);
    else  // book in requested
        context->set_current_book(
            requested_[choice - borrowed_.size() - 1].first);

    context->set_state(&Interface::ShowBook);
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
    using std::cout;
    using std::endl;

    ClearScreen();
    cout <<
" =========================== 主菜单 ===========================\n"
"欢迎你, 管理员 " << GetUserName(context) << "\n\n"
"[e] 查询书籍\n"
"[u] 查看用户       [n] 创建用户       [d] 删除用户"
"[p] 修改密码                                             [q] 退出\n";

    int choice = GetChoice("eundpq");
    switch (choice)
    {
        case 'e': context->set_state(&Interface::Query); break;
        case 'u': context->set_state(&Interface::ShowUser); break;
        case 'n': context->set_state(&Interface::CreateUser); break;
        case 'd': context->set_state(&Interface::DeleteUser); break;
        case 'p': context->set_state(&Interface::ChangePassword); break;
        case 'q': context->set_state(&Interface::WelcomeScreen); break;
    }
}

void AdminInterface::ShowUser(Context *context)
{
    using std::cout;
    using std::endl;

    DatabaseProxy *proxy = DatabaseProxy::Instance();

    ClearScreen();
    cout << " ========================== 查看用户 ==========================\n";

    UserID id = ReadUserID("用户ID: ");
    auto result = proxy->ReaderInfo(id);
    while (!result->next())
    {
        if (id == kInvalidUserID)  // perhaps an empty line
        {
            context->set_state(&Interface::MainMenu);
            return;
        }
        cout << "不存在ID为 " << id << "的用户\n";
        id = ReadUserID("用户ID: ");
        result = proxy->ReaderInfo(id);
    }

    // successful
    ClearScreen();
    cout << " ========================== 查看用户 ==========================\n"
         << "用户ID: " << id
         << "\n姓名: " << result->getString("name")
         << "\n\n所借书籍\n";

    int index = 1;
    {
        auto borrowed = proxy->QueryBorrowed(id);
        while (borrowed->next())
        {
            ShowThisCopy(index++, borrowed.get());
            cout << endl;
        }
    }
    if (index == 1)
        cout << "(无)\n\n";

    cout << "预约书籍\n";
    index = 1;
    {
        auto requested = proxy->QueryRequested(id);
        while (requested->next())
        {
            ShowThisCopy(index++, requested.get());
            cout << endl;
        }
    }
    if (index == 1)
        cout << "(无)\n\n";

    ContinueOrMainMenu(context);
}

void AdminInterface::CreateUser(Context *context)
{
    using std::cout;
    using std::endl;

    DatabaseProxy *proxy = DatabaseProxy::Instance();

    ClearScreen();
    cout << " ========================== 创建用户 ==========================\n";


    cout << "用户类型\n"
            "1: 读者   2: 管理员 (无借书权限)  3: 管理员 (有借书权限)\n";
    User type = static_cast<User>(GetChoice(3));

    UserID id = ReadUserID("用户ID: ");
    while (proxy->ReaderInfo(id)->next())
    {
        cout << "已存在ID为 " << id << "的用户\n";
        id = ReadUserID("用户ID: ");
    }
    if (id == kInvalidUserID)  // perhaps an empty line
    {
        context->set_state(&Interface::MainMenu);
        return;
    }

    std::string name = ReadLine("姓名: ");

    std::string password = ReadPassword("密码: ");
    if (password == ReadPassword("确认密码: "))
    {
        ClearScreen();
        cout << " ================== 创建用户 -> 确认 ==================\n"
             << "用户类型: " << kUserTypeNames[static_cast<int>(type) - 1]
             << "\n用户ID: " << id << endl;

        if (YesOrNo("确认要创建此用户吗? "))
        {
            if (DatabaseProxy::Instance()->CreateUser(type, id, name, password))
                cout << "创建成功\n";
            else
                cout << "创建失败\n";
        }
    }
    else
    {
        cout << "两次输入的密码不同\n";
    }

    ContinueOrMainMenu(context);
}

void AdminInterface::DeleteUser(Context *context)
{
    using std::cout;
    using std::endl;

    DatabaseProxy *proxy = DatabaseProxy::Instance();

    ClearScreen();
    cout << " ========================== 删除用户 ==========================\n";

    UserID id = ReadUserID("用户ID: ");
    auto result = proxy->ReaderInfo(id);
    while (id == context->user_id() || !result->next())  // prevent self delete
    {
        if (id == kInvalidUserID)  // perhaps an empty line
        {
            context->set_state(&Interface::MainMenu);
            return;
        }
        cout << "不存在ID为 " << id << "的用户\n";
        id = ReadUserID("用户ID: ");
        result = proxy->ReaderInfo(id);
    }

    ClearScreen();
    cout << " ================== 删除用户 -> 确认 ==================\n"
         << "用户ID: " << id
         << "\n姓名: " << result->getString("name") << endl;

    if (YesOrNo("确定要删除该用户吗? "))
    {
        if (proxy->DeleteUser(id))
            cout << "删除成功\n";
        else
            cout << "删除失败\n";
    }

    ContinueOrMainMenu(context);
}

AdminInterface * AdminInterface::Instance()
{
    static AdminInterface interface;
    return &interface;
}

void AdminReaderInterface::MainMenu(Context *context)
{
    using std::cout;

    ClearScreen();
    cout << " =========================== 主菜单 ===========================\n"
              << "欢迎你, 管理员 " << GetUserName(context) << "\n";

    int choice_num = PerpareMainMenuAndShowInfo(context);

    cout <<
"[编号] 查看对应书籍的详细信息\n"
"[e] 查询书籍\n"
"[b] 借书           [t] 还书           [r] 预约           [g] 预约取书\n"
"[u] 查看用户       [n] 创建用户       [d] 删除用户\n"
"[p] 修改密码                                             [q] 退出\n";

    int choice;
    if (GetChoice("ebtrgundpq", choice_num, choice) == Choice::CHAR)
    {
        switch (choice)
        {
            case 'e': context->set_state(&Interface::Query); break;
            case 'b': context->set_state(&Interface::BorrowBook); break;
            case 't': context->set_state(&Interface::ReturnBook); break;
            case 'r': context->set_state(&Interface::RequestBook); break;
            case 'g': context->set_state(&Interface::GetRequested); break;
            case 'p': context->set_state(&Interface::ChangePassword); break;
            case 'q': context->set_state(&Interface::WelcomeScreen); break;
            case 'u': context->set_state(&Interface::ShowUser); break;
            case 'n': context->set_state(&Interface::CreateUser); break;
            case 'd': context->set_state(&Interface::DeleteUser); break;
        }
    }
    else  // an index
    {
        ShowBookInfoFromMain(context, choice);
        return;
    }
}

AdminReaderInterface * AdminReaderInterface::Instance()
{
    static AdminReaderInterface interface;
    return &interface;
}


}  // namespace library_manager
