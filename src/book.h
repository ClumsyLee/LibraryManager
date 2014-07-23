#ifndef LM_BOOK_H_
#define LM_BOOK_H_

#include <ctime>

#include <ostream>
#include <string>
#include <vector>

#include "common.h"

namespace library_manager {

enum class BookStatus { ON_SHELF, LENT, AT_LOAN_DESK, LOST };

class BookCopy
{
 public:
    BookCopy(const BookID &id, int volume, int location);

    // only when the status is ON_SHELF
    bool Borrow(const UserID &reader_id);
    // only when the status is LENT
    bool Request(const UserID &reader_id);
    bool Return();

    void Update();  // should be done every day
                    // or some requests might be ignored

    // accessors
    BookID id() const { return id_; }
    int volume() const { return volume_; }
    int location() const { return location_; }

    BookStatus status() const { return status_; }
    UserID holder() const { return holder_; }
    std::time_t due_date() const { return due_date_; }
    int renew_times() const { return renew_times_; }
    const std::vector<UserID> & request_queue() const
    { return request_queue_; }
    bool recalled() const { return recalled_; }

 private:
    // basic info
    BookID id_;
    int volume_;  // 0 if no volumes
    int location_;

    // status
    BookStatus status_;
    UserID holder_;
    // meas due date when ON_SHELF
    //      the last day at loan dest when AT_LOAN_DESK
    std::time_t due_date_;
    int renew_times_;
    std::vector<UserID> request_queue_;
    bool recalled_;
};

class Book
{
 public:
    Book();

    static const std::vector<std::string> kLocations;


    // accessors
    const std::string & call_number() const { return call_number_; }

    const std::string & title() const { return title_; }
    const std::string & author() const { return author_; }
    const std::string & imprint() const { return imprint_; }
    const std::string & abstract() const { return abstract_; }
    const std::string & isbn() const { return isbn_; }

    const std::vector<BookCopy> & copies() const { return copies_; }

 private:
    std::string call_number_;

    std::string title_;
    std::string author_;
    std::string imprint_;
    std::string abstract_;
    std::string isbn_;

    std::vector<BookCopy> copies_;
};

// display a book
std::ostream & operator<<(std::ostream &os, const Book &book);

}  // namespace library_manager

#endif  // LM_BOOK_H_
