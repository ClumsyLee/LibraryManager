// #include "mysql_connection.h"
// #include <cppconn/driver.h>
#include "context.h"

namespace library_manager {

Context::Context(const std::string &host, const std::string &user_name,
                 const std::string &password)
    : interface_(Interface::GetInstance()),
      state_(&Interface::WelcomeScreen),
      connection_()
{
    // TODO: connect to sql sever
}

int Context::Run()
{
    try
    {
        while (true)
        {
            (interface_->*state_)(this);
        }
    }
    catch (ExitProgram &e)
    {
        return e.return_value();
    }
}

}  // namespace library_manager
