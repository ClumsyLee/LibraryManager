#include <cstdint>

#include "book.h"

namespace {

std::time_t Today()
{
    time_t timer = std::time(nullptr);
    std::tm *date = std::localtime(&timer);

    date->tm_sec = date->tm_min = date->tm_hour = 0;

    return std::mktime(date);
}

std::time_t nDaysLater(std::time_t begin, int n)
{
    std::tm *date = std::localtime(&begin);

    date->tm_sec = date->tm_min = date->tm_hour = 0;
    date->tm_mday += n;

    return std::mktime(date);
}

std::time_t nDaysLater(int n)
{
    return nDaysLater(std::time(nullptr), n);
}

// max diff that can handle: 68 years
int DaysDiff(std::time_t end, std::time_t begin)
{
    std::int32_t sec_diff = std::difftime(end, begin);
    return sec_diff / 86400;
}


}  // namespace


namespace library_manager {

const std::vector<int> kShortNotifyDays = {
    -1, 1, 3, 10, 40, 1039, 2038
};

const std::vector<int> kGeneralNotifyDays = {
    1, 3, 10, 100, 365, 1364, 2363
};


const std::vector<std::string> Book::kLocations = {
    "逸夫馆新书及常用书阅览室"
};

BookCopy::BookCopy(const BookID &id, int volume, int location,
                   BorrowType borrow_type)
        : id_(id),
          volume_(volume),
          location_(location),
          borrow_type_(borrow_type),
          status_(BookStatus::ON_SHELF),
          holder_(),
          begin_date_(),
          due_date_(),
          renew_times_(0),
          request_queue_(),
          recalled_(false)
{
}

bool BookCopy::Borrow(User *user)
{
    if (status_ == BookStatus::AT_LOAN_DESK &&
        request_queue_.front() == user->id())
    {
        // retrieve the book requested
        request_queue_.erase(request_queue_.begin());
    }
    else if (status_ != BookStatus::ON_SHELF)
    {
        std::cout << "该馆藏不在架上，无法借取\n";
        return false;
    }

    if (!(user->Borrow(id_)))  // error info will be printed inside
        return false;

    // borrowed
    status_ = BookStatus::LENT;
    holder_ = user->id();
    begin_date_ = Today();

    if (borrow_type_ == BorrowType::GENERAL)
        due_date_ = nDaysLater(user->general_period());
    else  // BorrowType::SHORT
        due_date_ = nDaysLater(user->short_period());

    renew_times_ = 0;
    recalled_ = false;

    return true;
}

bool BookCopy::Request(User *user)
{
    if (status_ != BookStatus::LENT)
    {
        std::cout << "只能预约当前被借出的馆藏\n";
        return false;
    }

    if (!(user->Request(id_)))  // error info will be printed inside
        return false;

    // requested
    if (!recalled_)  // haven't been recalled yet, do it now
    {
        std::time_t today = Today();

        if (DaysDiff(today, begin_date_) >= kMinHoldDays &&
            DaysDiff(due_date_, today) > kDaysAfterRecall)
        {
            // should advance due date
            due_date_ = nDaysLater(kDaysAfterRecall);
        }

        NotifyRecall();
        recalled_ = true;
    }
    request_queue_.push_back(user->id());

    return true;
}

bool BookCopy::Return(User *user)
{
    if (status_ != BookStatus::LENT)
    {
        std::cout << "只能还回被借出的馆藏\n";
        return false;
    }

    if (!(user->Return(id_)))  // error info will be printed inside
        return false;

    // returned
    if (request_queue_.empty())  // no one is waiting
    {
        status_ = BookStatus::ON_SHELF;
    }
    else  // sb. has requested
    {
        status_ = BookStatus::AT_LOAN_DESK;
        due_date_ = nDaysLater(kReserveDays);
        NotifyRequest();
    }
    return true;
}

void BookCopy::Update()
{
    if (status_ == BookStatus::LENT)
    {
        NotifyOverdueIfNeeded();
    }
    else if (status_ == BookStatus::AT_LOAN_DESK)
    {
        bool need_nodify = false;
        std::time_t today = Today();

        while (std::difftime(today, due_date_) > 0.0)
        {
            // another one missed the book
            request_queue_.erase(request_queue_.begin());
            if (request_queue_.empty())
            {
                // all the people missed the book
                status_ = BookStatus::ON_SHELF;
                return;
            }
            else  // move to next person
            {
                need_nodify = true;
                due_date_ = nDaysLater(due_date_, kReserveDays);
            }
        }
        // nodify the new person
        NotifyRequest();
    }
}

std::string BookCopy::StatusStr() const
{
    switch (status_)
    {
        case BookStatus::ON_SHELF:
        {
            return "在架上";
        }
        case BookStatus::LENT:
        {
            char buffer[20];
            std::strftime(buffer, 20, "到期 %F", std::localtime(&due_date_));
            std::string str(buffer);

            if (!request_queue_.empty())
            {
                str + " +" + std::to_string(request_queue_.size()) + " 预约";
            }

            return str;
        }
        case BookStatus::AT_LOAN_DESK:
        {
            return "在预约架上";
        }
        case BookStatus::LOST:
        {
            return "已丢失\n";
        }
    }

    return std::string();
}


void Book::Update()
{
    for (BookCopy &copy : copies_)
        copy.Update();
}


}  // namespace library_manager
