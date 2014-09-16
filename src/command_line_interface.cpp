#include "command_line_interface.h"

namespace library_manager {

CommandLineInterface::CommandLineInterface(int argc, const char *const argv[])
{
}

int CommandLineInterface::Run()
{
    return context_.Run();
}


}  // namespace library_manager
