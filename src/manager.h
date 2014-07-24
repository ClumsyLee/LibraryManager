#ifndef LM_MANAGER_H_
#define LM_MANAGER_H_

#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "book.h"
#include "common.h"
#include "io.h"
#include "user.h"

namespace library_manager {

class Manager
{
 public:
    Manager();
    Manager(const std::string &book_folder, const std::string &user_folder);

    void Welcome() const;

    void Login();
    void Logout();

    // access level 0
    void SearchBook();

    // access level 1
    void Borrow();
    void Return();
    void ShowStatus();

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

    // status
    bool has_login_;
    UserID user_now_;

    // for implement
    std::istringstream iss_;
    FileIO file_io_;

    // datas
    std::map<CallNum, Book> books_;
    std::map<BookID, CallNum> book_id_map_;
    std::map<UserID, std::shared_ptr<User>> users_;
};


}  // namespace library_manager

#endif  // LM_MANAGER_H_
