#ifndef COMMON_H_
#define COMMON_H_

#include <memory>
#include <string>

namespace library_manager {

typedef std::uint_least32_t UserID;
typedef std::uint_least64_t ISBN;
typedef std::string CopyID;
enum class User { NONE, READER, ADMIN, GUEST };

}  // namespace library_manager

#endif  // COMMON_H_
