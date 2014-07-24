#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "manager.h"

namespace library_manager {

Manager::Manager() : Manager("books", "users")
{
}

Manager::Manager(const std::string &book_folder, const std::string &user_folder)
        : user_now_(nullptr),

          iss_(),
          file_io_(book_folder, user_folder),

          books_(),
          book_id_map_(),
          users_()
{
    file_io_.Load(books_, book_id_map_, users_);
}

void Manager::Welcome() const
{
}

void Manager::GoodBye() const
{
}

void Manager::Login()
{
    using std::cout;

    if (user_now_)
    {
        cout << "您已经登录\n";
        return;
    }

    std::string user_id;
    Password password;

    // get id
    FeedStream("请输入用户名 (回车以退出): ");
    if (!(iss_ >> user_id))  // EOF or blank line
        return;

    auto user_iter = users_.find(user_id);
    if (user_iter == users_.end())
    {
        cout << user_id << ": 不存在该用户名\n";
        return;
    }

    // get password
    ReadLine("请输入密码: ", password);
    if (user_iter->second->VerifyPassword(password))
    {
        // login
        user_now_ = user_iter->second.get();
        Welcome();
    }
    else
    {
        cout << "密码错误\n";
    }
}

void Manager::Logout()
{
    if (user_now_)
    {
        GoodBye();
        user_now_ = nullptr;
    }
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

    BookID book_id;
    FeedStream("请输入要借阅的馆藏的条形码 (回车以退出): ");
    if (!(iss_ >> book_id))  // EOF or blank line
        return;

    try
    {
        auto book_pair = FindBook(book_id);

        if (book_pair.second.Borrow(user_now_))
        {
            cout << "借书成功\n";
            ShowBookCopy(book_id);
        }
        else
        {
            cout << "借书失败\n";
        }
    }
    catch (const std::out_of_range &oor)
    {
        cout << "无法找到条形码为 " << book_id << " 的图书\n";
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

    BookID book_id;
    FeedStream("请输入要归还的馆藏的条形码 (回车以退出): ");
    if (!(iss_ >> book_id))  // EOF or blank line
        return;

    try
    {
        auto book_pair = FindBook(book_id);

        if (book_pair.second.Return(user_now_))
        {
            cout << "还书成功\n";
        }
        else
        {
            cout << "还书失败\n";
        }
    }
    catch (const std::out_of_range &oor)
    {
        cout << "无法找到条形码为 " << book_id << " 的图书\n";
    }
}

void Manager::ShowStatus() const
{
    using std::cout;

    if (!CheckAccessLevel(1))
    {
        cout << "对不起，您没有借还书权限\n";
        return;
    }

    // show holding books
    cout << "您当前共借阅了" << user_now_->holding().size() << "本馆藏\n";
    for (const BookID &book_id : user_now_->holding())
    {
        ShowBookCopy(book_id);
    }

    // show request status
    cout << "\n您当前共预约了" << user_now_->requested().size() << "本馆藏\n";
    for (const BookID &book_id : user_now_->requested())
    {
        ShowBookCopy(book_id);
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

    BookID book_id;
    FeedStream("请输入要预约的馆藏的条形码 (回车以退出): ");
    if (!(iss_ >> book_id))  // EOF or blank line
        return;

    try
    {
        auto book_pair = FindBook(book_id);

        if (book_pair.second.Request(user_now_))
        {
            cout << "预约成功\n";
            ShowBookCopy(book_id);
        }
        else
        {
            cout << "预约失败\n";
        }
    }
    catch (const std::out_of_range &oor)
    {
        cout << "无法找到条形码为 " << book_id << " 的图书\n";
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

    cout << "开始录入新书目信息\n";

    CallNum call_number;
    while (true)
    {
        FeedStream("请输入索书号 (回车以退出): ");
        if (!(iss_ >> call_number))  // EOF or blank line
            return;
        if (books_.find(call_number) != books_.end())
        {
            cout << "索书号 " << call_number << " 已被占用\n";
        }
    }

    std::string title;
    if (!ReadLine("请输入题名: ", title))  // EOF
        return;

    std::string author;
    if (!ReadLine("请输入作者: ", author))  // EOF
        return;

    std::string imprint;
    if (!ReadLine("请输入出版社: ", imprint))  // EOF
        return;

    std::string abstract;
    if (!ReadParagraph("请输入摘要 (以空行结束): ", abstract))  // EOF
        return;

    std::string isbn;
    if (!ReadLine("请输入ISBN: ", isbn))  // EOF
        return;

    Book new_book(call_number, title, author, imprint, abstract, isbn);

    cout << "您输入的新书目信息如下:\n\n"
         << new_book << std::endl;
    if (YesOrNo("\n要添加此书目吗? (y/n): "))
        books_.emplace(call_number, new_book);
}

void Manager::AddCopy()
{
    using std::cout;

    if (!CheckAccessLevel(3))
    {
        cout << "对不起，您没有增添馆藏的权限\n";
        return;
    }

    cout << "开始录入新馆藏信息\n";

    CallNum call_number;
    std::map<CallNum, Book>::iterator book_iter;
    while (true)
    {
        FeedStream("请输入索书号 (回车以退出): ");
        if (!(iss_ >> call_number))  // EOF or blank line
            return;
        book_iter = books_.find(call_number);
        if (book_iter == books_.end())
        {
            cout << "索书号 " << call_number << " 已被占用\n";
        }
    }

    BookID book_id;
    while (true)
    {
        FeedStream("请输入条形码 (回车以退出): ");
        if (!(iss_ >> book_id))  // EOF or blank line
            return;
        if (book_id_map_.find(book_id) != book_id_map_.end())
        {
            cout << "条形码 " << book_id << " 已被占用\n";
        }
    }

    int volume;
    FeedStream("请输入卷次 (非数字以退出): ");
    if (!(iss_ >> volume))  // cannot read number
        return;
    if (volume < 0)
        volume = 0;

    int location;
    ShowLocations(cout);
    while (true)
    {
        FeedStream("请输入书籍所在位置 (非数字以退出): ");
        if (!(iss_ >> location))  // cannot read number
            return;
        if (location < 0 || location >= kLocations.size())
        {
            cout << location << ": 无效的位置编号\n";
        }
    }

    int borrow_type;
    while (true)
    {
        FeedStream("请输入馆藏借期类型 (0: 一般, 1: 短期, 非数字以退出): ");
        if (!(iss_ >> borrow_type))  // cannot read number
            return;
        if (borrow_type < 0 || borrow_type >= 1)
        {
            cout << borrow_type << ": 无效的借期类型\n";
        }
    }

    cout << "您输入的新馆藏信息如下:\n"
         << "索书号: " << call_number
                      << " (" << book_iter->second.title() << ")\n"
         << "条形码: " << book_id
         << "\n卷次: " << volume
         << "\n位置: " << kLocations[location]
         << "\n借期类型: "  << (borrow_type ? "短期\n": "一般\n");

    if (YesOrNo("要添加此馆藏吗? (y/n): "))
    {
        BookCopy new_copy(book_id, volume, location,
                          static_cast<BorrowType>(borrow_type));
        book_iter->second.copies().push_back(new_copy);
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

std::pair<Book &, BookCopy &> Manager::FindBook(const BookID &book_id)
{
    Book &book = books_.at(book_id_map_.at(book_id));

    auto copy_iter = std::find(book.copies().begin(), book.copies().end(),
                               book_id);
    if (copy_iter == book.copies().end())  // should be here, not scientific
        throw std::out_of_range("在对应书目下找不到该副本");

    return {book, *copy_iter};
}


std::pair<const Book &,
          const BookCopy &> Manager::FindBook(const BookID &book_id) const
{
    const Book &book = books_.at(book_id_map_.at(book_id));

    auto copy_iter = std::find(book.copies().begin(), book.copies().end(),
                               book_id);
    if (copy_iter == book.copies().end())  // should be here, not scientific
        throw std::out_of_range("在对应书目下找不到该副本");

    return {book, *copy_iter};
}

void Manager::ShowBookCopy(const BookID &book_id) const
try
{
    static std::string separator = std::string('=', 79);
    static std::string sub_separator('.', 79);

    using std::cout;
    using std::endl;

    auto book_pair = FindBook(book_id);
    cout << separator << endl
         << book_pair.first.title() << endl
         << sub_separator << endl
         << "条形码: " << book_id << "        "
         << book_pair.second.StatusStr() << "        "
         << "索书号: " << book_pair.first.call_number() << endl;
}
catch (const std::out_of_range &oor)
{
    std::cerr << "检索 " << book_id << "失败: "
              << oor.what() << std::endl;
}



bool Manager::CheckAccessLevel(int min_level) const
{
    int level = user_now_ ?
                user_now_->access_level() :
                0;

    return level >= min_level;
}



}  // namespace library_manager
