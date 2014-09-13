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
    class ExitProgram : public std::exception
    {
     public:
        explicit ExitProgram(int return_value)
            : return_value_(return_value) {}
        int return_value() const { return return_value_; }
     private:
        int return_value_;
    };

    Context();

    int Run();

    void set_interface(Interface *interface) { interface_ = interface; }
    void set_state(State state) { state_ = state; }

 private:
    Interface *interface_;
    State state_;
};

}  // namespace library_manager

#endif  // CONTEXT_H_
