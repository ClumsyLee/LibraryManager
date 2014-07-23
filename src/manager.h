#ifndef LM_MANAGER_H_
#define LM_MANAGER_H_

#include <map>
#include <sstream>

#include "book.h"
#include "common.h"
#include "interface.h"
#include "io.h"
#include "user.h"

namespace library_manager {

class Manager
{
 public:
    Manager();

    void Welcome() const;

    void Login();
    void Logout();

    // access level 0
    void SearchBook();

    // access level 1
    void Borrow();
    void Return();

    // access level 2
    void Request();

    // access level 3
    void AddBook();
    void AddCopy();
    void AddUser();

    // access level 4
    void DeleteBook();
    void DeleteCopy();
    void DeleteUser();

 private:
    bool FeedStream(const std::string &prompt);
    bool ReadLine(const std::string &prompt, std::string &line) const;

    bool CheckAccessLevel(int min_level) const;

    std::unique_ptr<User> user_;
    Interface *interface_;
    std::istringstream iss_;
    FileIO file_io_;

    BookCollection books_;
};


}  // namespace library_manager

#endif  // LM_MANAGER_H_
