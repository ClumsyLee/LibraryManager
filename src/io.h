#ifndef LM_IO_H_
#define LM_IO_H_

#include <memory>
#include <string>

#include "book.h"
#include "user.h"

namespace library_manager {

// input function
bool ReadLine(const std::string &prompt, std::string &line);
bool ReadParagraph(const std::string &prompt, std::string &line);
bool YesOrNo(const std::string &prompt);

class FileBasic
{
 public:
    FileBasic(const std::string &book_folder, const std::string &user_folder)
            : book_folder_(book_folder),
              user_folder_(user_folder) {}

    const std::string & book_folder() const { return book_folder_; }
    const std::string & user_folder() const { return user_folder_; }

 private:
    std::string book_folder_;
    std::string user_folder_;
};

class FileLoader : virtual public FileBasic
{
 public:
    FileLoader(const std::string &book_folder, const std::string &user_folder)
            : FileBasic(book_folder, user_folder) {}

    bool Load(std::map<CallNum, Book> &books,
              std::map<BookID, CallNum> &book_id_map,
              std::map<UserID, std::shared_ptr<User>> &users);
};

class FileSaver : virtual public FileBasic
{
 public:
    FileSaver(const std::string &book_folder, const std::string &user_folder)
            : FileBasic(book_folder, user_folder) {}

    bool Save(const std::map<CallNum, Book> &books,
              const std::map<BookID, CallNum> &book_id_map,
              const std::map<UserID, std::shared_ptr<User>> &users);
};

class FileIO : public FileLoader, public FileSaver
{
 public:
    FileIO(const std::string &book_folder, const std::string &user_folder)
            : FileBasic(book_folder, user_folder),
              FileLoader(book_folder, user_folder),
              FileSaver(book_folder, user_folder) {}
};

}  // namespace library_manager

#endif  // LM_IO_H_
