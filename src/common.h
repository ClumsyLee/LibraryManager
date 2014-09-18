#ifndef COMMON_H_
#define COMMON_H_

#include <memory>
#include <string>

namespace library_manager {

typedef std::uint_least32_t UserID;
typedef std::uint_least64_t ISBN;
typedef std::string CopyID;
typedef std::uint_least64_t PhoneNum;
typedef std::string CallNum;
enum class User { READER, ADMIN };

const int kLoanPeriod = 90;

}  // namespace library_manager

#endif  // COMMON_H_
