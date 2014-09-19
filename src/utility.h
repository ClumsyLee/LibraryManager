#ifndef UTILITY_H_
#define UTILITY_H_

#include <string>
#include "common.h"

namespace library_manager {

enum Choice { CHAR, INDEX };

const UserID kInvalidUserID = 0;

void ClearScreen();

// readline
std::string ReadLine(const std::string &prompt);
UserID ReadUserID(const std::string &prompt);
std::string ReadPassword(const std::string &prompt);

// choice
bool YesOrNo(const std::string &prompt);
Choice GetChoice(const std::string &chars, int max_index, int &choice);
int GetChoice(const std::string &chars);
int GetChoice(int max_index);

}  // namespace library_manager

#endif  // UTILITY_H_
