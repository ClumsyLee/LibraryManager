#ifndef USER_H_
#define USER_H_

#include <memory>
#include "manager.h"

namespace library_manager {

class User
{
 public:
    User();

    virtual std::unique_ptr<Manager> GetManager() = 0;

 private:

};

class Reader : public User
{
 public:
    Reader();

 private:

};

class Admin : public User
{
 public:
    Admin();

 private:

};

class Guest : public User
{
 public:
    Guest();

 private:

};

}

#endif  // USER_H_
