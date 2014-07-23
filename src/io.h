#ifndef LM_IO_H_
#define LM_IO_H_

#include <memory>

#include "book.h"
#include "user.h"

namespace library_manager {

class FileLoader
{
 public:
    FileLoader();

    bool Load(BookCollection &books);
    std::unique_ptr<User> Load(const UserID &id);

 private:

};

class FileSaver
{
 public:
    FileSaver();

    bool Save(const BookCollection &books);
    bool Save(const User *user);

 private:

};

class FileIO : public FileLoader, public FileSaver
{
 public:
    FileIO();

 private:

};

}  // namespace library_manager

#endif  // LM_IO_H_
