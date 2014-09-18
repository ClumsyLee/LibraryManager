#include <iostream>
#include <mysql/mysql.h>
#include "command_line_interface.h"

int main(int argc, char *argv[])
{
    if (mysql_library_init(0, NULL, NULL))
    {
        std::cerr << "could not initialize MySQL library\n";
        return 1;
    }
    library_manager::CommandLineInterface cli(argc, argv);

    mysql_library_end();
    return cli.Run();
}
