#include "context.h"
#include "interface.h"

namespace library_manager {

Context::Context()
    : interface_(Interface::Instance()),
      state_(&Interface::WelcomeScreen)
{
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
