#include "command_line_interface.h"
#include <iostream>

int main(int argc, char *argv[])
{
    library_manager::CommandLineInterface cli(argc, argv);
    return cli.Run();
}
