#ifndef LM_USER_H_
#define LM_USER_H_

#include <memory>
#include <iostream>
#include <string>

#include "common.h"

namespace library_manager {

typedef std::string Password;

class User
{
 public:
    virtual ~User() = default;

    bool VerifyPassword(const Password &password) const;

    // will print error info to std::cout
    // remove entry in requested
    bool Borrow(const BookID &book_id);
    bool Request(const BookID &book_id);
    bool Return(const BookID &book_id);

    virtual void Display(std::ostream &os) const = 0;
    virtual bool Save(std::ostream &os) const;
    virtual bool Load(std::istream &is);

    // info about borrowing
    virtual int access_level() const { return 0; }

    virtual int max_borrow() const { return 30; }
    virtual int max_request() const { return 3; }

    virtual int general_period() const { return 30; }
    virtual int general_renewal_allowed() const { return 3; }

    virtual int short_period() const { return 7; }
    virtual int shrot_renewal_allowed() { return 3; }


    // accessors
    const std::string & id() const { return id_; }
    const std::vector<BookID> & holding() const { return holding_; }
    const std::vector<BookID> & requested() const { return requested_; }

 private:
    std::string id_;
    Password password_;
    std::vector<BookID> holding_;
    std::vector<BookID> requested_;
};

class UserFactory
{
 public:
    virtual ~UserFactory() = default;

    virtual std::unique_ptr<User> Create(const UserID &id) = 0;
};


class Student : public User
{
 public:
    virtual ~Student() = default;

 private:

};

class StudentFactory : public UserFactory
{
 public:
    virtual ~StudentFactory() = default;

    virtual std::unique_ptr<User> Create(const UserID &id);
 private:

};

class Administrator : public User
{
 public:
    virtual ~Administrator() = default;

};

class AdministratorFactory : public UserFactory
{
 public:
    virtual ~AdministratorFactory() = default;

    virtual std::unique_ptr<User> Create(const UserID &id);

 private:

};

}  // namespace library_manager

#endif  // LM_USER_H_
