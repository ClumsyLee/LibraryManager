#include <algorithm>

#include "user.h"

namespace  library_manager{

bool User::VerifyPassword(const Password &password) const
{
    return password_ == password;
}

bool User::Borrow(const BookID &book_id)
{
    if (holding_.size() >= max_borrow())
    {
        std::cout << "该用户所借书籍数目已达到上限（" << max_borrow() << "本），"
                     "无法继续借书\n";
        return false;
    }

    holding_.push_back(book_id);

    // remove entry if in requested list
    auto iter = std::find(requested_.begin(), requested_.end(), book_id);
    if (iter != requested_.end())
        requested_.erase(iter);

    return true;
}

bool User::Request(const BookID &book_id)
{
    if (requested_.size() >= max_request())
    {
        std::cout << "该用户预约图书数目已达到上限（" << max_request() << "本），"
                     "无法进行预约\n";
        return false;
    }

    requested_.push_back(book_id);
    return true;
}

bool User::Return(const BookID &book_id)
{
    auto iter = std::find(holding_.begin(), holding_.end(), book_id);
    if (iter == holding_.end())
    {
        std::cout << "该馆藏未由该用户借出，无法归还\n";
        return false;
    }

    holding_.erase(iter);
    return true;
}

}  // namespace library_manager
