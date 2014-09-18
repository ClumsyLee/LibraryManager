#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>
#include <vector>
#include <utility>
#include "common.h"

namespace sql {
    class Connection;
}  // namespace sql

namespace library_manager {

class Context;

class Interface
{
 public:
    Interface() = default;
    virtual ~Interface() = default;

    void WelcomeScreen(Context *context);
    void Login(Context *context);
    virtual void MainMenu(Context *context);
    virtual void Query(Context *context);
    virtual void ShowBook(Context *context);
    virtual void BorrowBook(Context *context);
    virtual void RequestBook(Context *context);
    virtual void ReturnBook(Context *context);
    virtual void GetRequested(Context *context);

    static Interface * Instance();

 private:
    static void GetValidUser(UserID &user_id, User &user);
};

class ReaderInterface : public Interface
{
 public:
    ReaderInterface() = default;
    virtual ~ReaderInterface() = default;

    virtual void MainMenu(Context *context);
    virtual void BorrowBook(Context *context);
    virtual void RequestBook(Context *context);
    virtual void GetRequested(Context *context);

    static ReaderInterface * Instance();

 private:
    typedef std::pair<ISBN, CopyID> TempBook;

    void ShowReaderInfo() const;
    void ShowBorrowed();
    void ShowRequested();

    UserID user_id_;
    std::vector<TempBook> borrowed_;
    std::vector<TempBook> requested_;
};

class AdminInterface : public Interface
{
 public:
    AdminInterface() = default;
    virtual ~AdminInterface() = default;

    virtual void MainMenu(Context *context);
    virtual void BorrowBook(Context *context);
    virtual void GetRequested(Context *context);

    static AdminInterface * Instance();
};

}  // namespace library_manager

#endif  // INTERFACE_H_
