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

        if (books_.find(call_number) == books_.end())
            break;
        cout << "索书号 " << call_number << " 已被占用\n";
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
        if (book_iter != books_.end())
            break;

        cout << "不存在索书号为 " << call_number << " 的书目\n";
    }

    BookID book_id;
    while (true)
    {
        FeedStream("请输入条形码 (回车以退出): ");
        if (!(iss_ >> book_id))  // EOF or blank line
            return;

        if (book_id_map_.find(book_id) == book_id_map_.end())
            break;

        cout << "条形码 " << book_id << " 已被占用\n";
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

        if (location >= 0 && location < kLocations.size())
            break;

        cout << location << ": 无效的位置编号\n";
    }

    int borrow_type;
    while (true)
    {
        FeedStream("请输入馆藏借期类型 (0: 一般, 1: 短期, 非数字以退出): ");
        if (!(iss_ >> borrow_type))  // cannot read number
            return;

        if (borrow_type == 0 || borrow_type == 1)
            break;

        cout << borrow_type << ": 无效的借期类型\n";
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
        // update id map
        book_id_map_.emplace(book_id, call_number);
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

    int choice;
    cout << "0: 管理员, 1: 学生\n";
    FeedStream("请选择新建用户的类型 (非数字以退出): ");
    if (!(iss_ >> choice))  // cannot read a choice
        return;
    if (choice < 0 || choice >= 2)
    {
        cout << "无效的用户类型\n";
        return;
    }

    // choose factory
    std::unique_ptr<UserFactory> factory;
    if (choice == 0)
        factory.reset(new AdministratorFactory);
    else  // choice == 1
        factory.reset(new StudentFactory);

    // get id
    UserID user_id;
    if (ReadLine("请输入用户名: ", user_id))  // EOF
        return;

    std::shared_ptr<User> new_user = factory->Create(user_id);
    if (!new_user)  // fail to add
        return;

    new_user->Display(cout);
    if (YesOrNo("确定要添加该用户吗? (y/n): "))
        users_.emplace(user_id, new_user);
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

    CallNum call_number;
    std::map<CallNum, Book>::iterator book_iter;
    while (true)
    {
        FeedStream("请输入索书号 (回车以退出): ");
        if (!(iss_ >> call_number))  // EOF or blank line
            return;

        book_iter = books_.find(call_number);
        if (book_iter != books_.end())
            break;

        cout << "不存在索书号为 " << call_number << " 的书目\n";
    }

    if (!(book_iter->second.copies().empty()))
    {
        cout << "删除失败: 不能删除副本非空的书目\n";
        return;
    }

    std::string prompt = std::string("您确定要删除 ") + call_number
            + " ("  + book_iter->second.title() + ") 吗? (y/n): ";
    if (YesOrNo(prompt))
        books_.erase(book_iter);
}

void Manager::DeleteCopy()
{
    using std::cout;

    if (!CheckAccessLevel(4))
    {
        cout << "对不起，您没有删除馆藏的权限\n";
        return;
    }

    BookID book_id;
    while (true)
    {
        FeedStream("请输入条形码 (回车以退出): ");
        if (!(iss_ >> book_id))  // EOF or blank line
            return;

        if (book_id_map_.find(book_id) != book_id_map_.end())
            break;

        cout << "不存在条形码为 " << book_id << " 的馆藏\n";
    }

    try
    {
        int index;
        auto book_pair = FindBook(book_id, &index);

        BookStatus status = book_pair.second.status();
        if (status != BookStatus::ON_SHELF && status != BookStatus::LOST)
        {
            cout << "删除失败: 只能删除在架上或丢失的图书\n";
            return;
        }
        if (YesOrNo("你真的要删除该馆藏吗? (y/n):"))
        {
            auto &copies = book_pair.first.copies();
            copies.erase(copies.begin() + index);
            // remove from id map
            book_id_map_.erase(book_id);
        }
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "删除失败: " << oor.what() << std::endl;
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

    UserID user_id;
    if (!ReadLine("请输入要删除的用户的用户名: ", user_id))
        return;

    auto user_iter = users_.find(user_id);
    if (!user_iter->second->holding().empty() ||
        !user_iter->second->requested().empty())
    {
        cout << "删除失败: 该用户持有或预约了馆藏\n";
        return;
    }

    if (YesOrNo("确认删除用户吗 (y/n): "))
        users_.erase(user_iter);
}

std::pair<Book &, BookCopy &> Manager::FindBook(const BookID &book_id,
                                                int *index)
{
    Book &book = books_.at(book_id_map_.at(book_id));

    for (int i = 0; i < book.copies().size(); i++)
    {
        if (book.copies()[i].id() == book_id)
        {
            if (index)
                *index = i;
            return {book, book.copies()[i]};
        }
    }
    throw std::out_of_range("在对应书目下找不到该副本");
}


std::pair<const Book &,
          const BookCopy &> Manager::FindBook(const BookID &book_id,
                                              int *index) const
{
    const Book &book = books_.at(book_id_map_.at(book_id));

    for (int i = 0; i < book.copies().size(); i++)
    {
        if (book.copies()[i].id() == book_id)
        {
            if (index)
                *index = i;
            return {book, book.copies()[i]};
        }
    }
    throw std::out_of_range("在对应书目下找不到该副本");
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
