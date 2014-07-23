#ifndef LM_INTERFACE_H_
#define LM_INTERFACE_H_

#include <istream>
#include <string>

namespace library_manager {

class Interface
{
 public:
    Interface() = default;
    virtual ~Interface() = default;

    virtual bool ReadLine(const std::string &prompt, std::string &line);
};

}  // namespace library_manager

#endif  // LM_INTERFACE_H_
