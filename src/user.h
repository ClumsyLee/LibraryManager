#ifndef LM_USER_H_
#define LM_USER_H_

#include <string>

namespace library_manager {

class User
{
 public:
    virtual ~User() = default;

    virtual int access_level() const = 0;

    const std::string & name() const;

};

}  // namespace library_manager

#endif  // LM_USER_H_
