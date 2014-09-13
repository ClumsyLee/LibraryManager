#ifndef COMMAND_LINE_INTERFACE_H_
#define COMMAND_LINE_INTERFACE_H_

namespace library_manager {

class CommandLineInterface
{
 public:
    CommandLineInterface(int argc, const char *const argv[]);
    int Run();
 private:

};

}  // namespace library_manager

#endif  // COMMAND_LINE_INTERFACE_H_
