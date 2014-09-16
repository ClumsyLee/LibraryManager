#ifndef COMMAND_LINE_INTERFACE_H_
#define COMMAND_LINE_INTERFACE_H_

#include "context.h"

namespace library_manager {

class CommandLineInterface
{
 public:
    CommandLineInterface(int argc, const char *const argv[]);
    int Run();
 private:
    Context context_;
};

}  // namespace library_manager

#endif  // COMMAND_LINE_INTERFACE_H_
