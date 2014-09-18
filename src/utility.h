#ifndef UTILITY_H_
#define UTILITY_H_

#include <string>
#include "common.h"

namespace library_manager {

enum Choice { CHAR, INDEX };

void ClearScreen();

// readline
std::string ReadLine(const std::string &promt);
UserID ReadUserID(const std::string &promt);
std::string ReadPassword(const std::string &promt);

// choice
bool YesOrNo(const std::string &promt);
Choice GetChoice(const std::string &chars, int max_index, int &choice);
int GetChoice(const std::string &chars);

}  // namespace library_manager

#endif  // UTILITY_H_
