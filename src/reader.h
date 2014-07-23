#ifndef LM_READER_H_
#define LM_READER_H_

#include <string>
#include <vector>
#include "common.h"

namespace library_manager {

class Reader
{
 public:
    Reader();

 private:
    std::string name_;
    std::vector<BookID> books_hold_;
};

}  // namespace library_manager

#endif  // LM_READER_H_
