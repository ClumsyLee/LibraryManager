#ifndef LM_MANAGER_H_
#define LM_MANAGER_H_

#include <memory>

#include "book.h"
#include "common.h"
#include "interface.h"
#include "user.h"

namespace library_manager {

class Manager
{
 public:
    Manager();

    void Login(User *user);
    void Logout();

    bool Borrow(const BookID &id);
    void Return(const BookID &id);
    bool Request(const BookID &id);

    // Book management
    bool AddBook();
    bool AddCopy();
    bool DeleteBook();

    // User Management
    bool AddUser(const UserID &name, const Password &password);
    bool DeleteUser(const UserID &id);

 private:
    User *user_;
    Interface *interface_;

    std::unique_ptr<User> LoadUser(const UserID &name);
    bool SaveUser(const User *user);
};


}  // namespace library_manager

#endif  // LM_MANAGER_H_
