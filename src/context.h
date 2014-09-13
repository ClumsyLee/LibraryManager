#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <exception>
#include <memory>
#include <string>

namespace sql {
    class Connection;
}  // namespace sql

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

    Context(const std::string &host, const std::string &user_name,
            const std::string &password);

    int Run();

    void SetInterface(Interface *new_interface) { interface_ = new_interface; }
    void SetState(State new_state) { state_ = new_state; }
    sql::Connection * connection() { return connection_.get(); }

 private:
    Interface *interface_;
    State state_;
    std::unique_ptr<sql::Connection> connection_;
};

}  // namespace library_manager

#endif  // CONTEXT_H_
