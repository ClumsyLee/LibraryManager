#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <exception>
#include <string>
#include "common.h"

namespace library_manager {

class Interface;

class Context
{
 public:
    typedef void (Interface::*State)(Context *);

    Context();

    int Run();

    UserID user_id() const { return user_id_; }
    const std::string & keyword() const { return keyword_; }
    ISBN current_book() const { return current_book_; }

    void set_interface(Interface *interface) { interface_ = interface; }
    void set_state(State state) { state_ = state; }
    void set_user_id(UserID user_id) { user_id_ = user_id; }
    void set_keyword(const std::string &keyword) { keyword_ = keyword; }
    void set_current_book(ISBN isbn) { current_book_ = isbn; }

 private:
    Interface *interface_;
    State state_;
    UserID user_id_;
    std::string keyword_;
    ISBN current_book_;
};

}  // namespace library_manager

#endif  // CONTEXT_H_
