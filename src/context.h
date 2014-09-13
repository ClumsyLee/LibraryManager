#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <memory>
#include "interface.h"

namespace sql {
    class Connection;
}  // namespace sql

namespace library_manager {

class Context
{
 public:
    typedef void (Interface::*State)();

    Context();

    int Run();

    void SetState(State new_state);
    void SetInterface(Interface *new_interface);

 private:
    Interface *interface_;
    State state_;
    std::unique_ptr<sql::Connection> connection_;
};

}  // namespace library_manager

#endif  // CONTEXT_H_
