#ifndef LM_COMMAND_LINE_INTERFACE_H_
#define LM_COMMAND_LINE_INTERFACE_H_

#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "interface.h"
#include "manager.h"

namespace library_manager {

class CommandLineInterface : public Interface
{
 public:
    CommandLineInterface();
    virtual ~CommandLineInterface() = default;

    virtual int Run(int argc, const char* const argv[]);


 private:
    typedef std::function<void(CommandLineInterface &)> ParseFunction;
    typedef std::pair<std::string, ParseFunction> Command;

    // return true if need to exit
    bool ParseAndRunCommand();

    void Save();
    void Load();

    void PrintHelpInfo() const;

    bool GetMenuChoice(int max_number, int &choice);
    bool GetYesNoChoice(const char *prompt) const;
    bool GetYesNoChoice(const std::string &prompt) const
    {
        return GetYesNoChoice(prompt.c_str());
    }

    bool ReadLine(const char *prompt, std::string &line) const;
    bool ReadLineIntoStream(const char *prompt) const;
    friend char * CommandGenerator(const char *text, int state);

    static std::vector<Command> commands_;

    std::string prompt_;
    mutable std::istringstream command_stream_;
    Manager manager_;

    bool interactive_mode;
};

}  // namespace library_manager

#endif  // LM_COMMAND_LINE_INTERFACE_H_
