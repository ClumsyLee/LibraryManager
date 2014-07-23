#include <memory>

#include "command_line_interface.h"

int main(int argc, char *argv[])
{
    using namespace library_manager;

    std::unique_ptr<Interface> interface(new CommandLineInterface);

    return interface->Run(argc, argv);
}
