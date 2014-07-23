#include "book.h"

namespace library_manager {

const std::vector<std::string> Book::kLocations = {
    "逸夫馆新书及常用书阅览室"
};

BookCopy::BookCopy(const BookID &id, int volume, int location)
        : id_(id),
          volume_(volume),
          location_(location),
          status_(BookStatus::ON_SHELF),
          holder_(),
          due_date_(),
          renew_times_(0),
          request_queue_(),
          recalled_(false)
{
}

bool BookCopy::Borrow(const UserID &user_id)
{
    if (status_ != BookStatus::ON_SHELF)
        return false;
}

}  // namespace library_manager
