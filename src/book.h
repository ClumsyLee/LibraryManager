#ifndef LM_BOOK_H_
#define LM_BOOK_H_

#include <ctime>

#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "common.h"
#include "user.h"

namespace library_manager {

enum class BookStatus { ON_SHELF, LENT, AT_LOAN_DESK, LOST };
enum class BorrowType { GENERAL, SHORT };

class BookCopy
{
 public:
    BookCopy(const BookID &id, int volume, int location,
             BorrowType borrow_type);

    // only when the status is ON_SHELF or AT_LOAN_DESK
    bool Borrow(User *user);
    // only when the status is LENT
    bool Request(User *user);
    bool Return(User *user);

    void Update();  // should be done every day
                    // or some requests might be ignored
    std::string StatusStr() const;

    // accessors
    BookID id() const { return id_; }
    int volume() const { return volume_; }
    int location() const { return location_; }
    BorrowType borrow_type() const { return borrow_type_; }

    BookStatus status() const { return status_; }
    UserID holder() const { return holder_; }
    std::time_t due_date() const { return due_date_; }
    int renew_times() const { return renew_times_; }
    const std::vector<UserID> & request_queue() const
    { return request_queue_; }
    bool recalled() const { return recalled_; }

    static constexpr int kMinHoldDays = 28;
    static constexpr int kDaysAfterRecall = 7;
    static constexpr int kReserveDays = 3;

    static const std::vector<int> kShortNotifyDays;
    static const std::vector<int> kGeneralNotifyDays;

 private:
    void NotifyRecall();
    void NotifyRequest();
    void NotifyOverdueIfNeeded();

    // basic info
    BookID id_;
    int volume_;  // 0 if no volumes
    int location_;
    BorrowType borrow_type_;

    // status
    BookStatus status_;
    UserID holder_;
    std::time_t begin_date_;
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

    void Update();  // update all the copies

    // accessors
    const CallNum & call_number() const { return call_number_; }

    const std::string & title() const { return title_; }
    const std::string & author() const { return author_; }
    const std::string & imprint() const { return imprint_; }
    const std::string & abstract() const { return abstract_; }
    const std::string & isbn() const { return isbn_; }

    const std::vector<BookCopy> & copies() const { return copies_; }

 private:
    CallNum call_number_;

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
