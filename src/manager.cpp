#include <iostream>

#include "manager.h"

namespace library_manager {

Manager::Manager() : user_(nullptr),
                     interface_(nullptr),
                     books_() {}

void Manager::Welcome() const
{
}

void Manager::Login()
{
    using std::cout;

    std::string user_id;
    Password password;

    FeedStream("请输入用户名: ");
    if (!(iss_ >> user_id))  // EOF
        return;
    user_ = file_io_.Load(user_id);

    if (!user_)
    {
        cout << user_id << ": 不存在该用户名\n";
        user_.reset(nullptr);
        return;
    }

    ReadLine("请输入密码: ", password);
    if (user_->VerifyPassword(password))
    {
        Welcome();
    }
    else
    {
        cout << "密码错误\n";
    }
}

void Manager::Logout()
{
    user_.reset(nullptr);
}

// access level 0
void SearchBook()
{

}

// access level 1
void Manager::Borrow()
{
    using std::cout;

    if (!CheckAccessLevel(1))
    {
        cout << "对不起，您没有借书权限\n";
        return;
    }



}

void Manager::Return()
{
    using std::cout;

    if (!CheckAccessLevel(1))
    {
        cout << "对不起，您没有还书权限\n";
        return;
    }


}


// access level 2
void Manager::Request()
{
    using std::cout;

    if (!CheckAccessLevel(2))
    {
        cout << "对不起，您没有预约权限\n";
        return;
    }

}


// access level 3
void Manager::AddBook()
{
    using std::cout;

    if (!CheckAccessLevel(3))
    {
        cout << "对不起，您没有增添书目的权限\n";
        return;
    }


}

void Manager::AddCopy()
{
    using std::cout;

    if (!CheckAccessLevel(3))
    {
        cout << "对不起，您没有增添馆藏的权限\n";
        return;
    }
}

void Manager::AddUser()
{
    using std::cout;

    if (!CheckAccessLevel(3))
    {
        cout << "对不起，您没有新建用户的权限\n";
        return;
    }
}

// access level 4
void Manager::DeleteBook()
{
    using std::cout;

    if (!CheckAccessLevel(4))
    {
        cout << "对不起，您没有删除书目的权限\n";
        return;
    }

}

void Manager::DeleteCopy()
{
    using std::cout;

    if (!CheckAccessLevel(4))
    {
        cout << "对不起，您没有删除馆藏的权限\n";
        return;
    }

}

void Manager::DeleteUser()
{
    using std::cout;

    if (!CheckAccessLevel(4))
    {
        cout << "对不起，您没有删除用户的权限\n";
        return;
    }

}




bool Manager::CheckAccessLevel(int min_level) const
{
    int level = user_ ?
                user_->access_level() :
                0;

    return level >= min_level;
}



}  // namespace library_manager
