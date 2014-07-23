#ifndef LM_USER_H_
#define LM_USER_H_

#include <string>

#include "common.h"

namespace library_manager {

class User
{
 public:
    virtual ~User() = default;

    bool VerifyPassword(const Password &password);

    virtual int access_level() const = 0;

    const std::string & name() const { return name_; }
    const std::vector<BookID> & books_holding() const { return books_holding_; }

 private:
    std::string name_;
    Password password_;
    std::vector<BookID> books_holding_;
};

}  // namespace library_manager

#endif  // LM_USER_H_
