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
    virtual void ChangePassword(Context *context);

    virtual void ShowUser(Context *context);
    virtual void CreateUser(Context *context);
    virtual void RemoveUser(Context *context);

    virtual void CreateBook(Context *context);
    virtual void CreateCopy(Context *context);
    virtual void RemoveBook(Context *context);
    virtual void RemoveCopy(Context *context);

    virtual void LostCopy(Context *context);


    static Interface * Instance();

 protected:
    virtual void ShowCopiesOfBook(Context *context);
    void ContinueOrMainMenu(Context *context);
    std::string GetUserName(Context *context);

    bool GetExistISBN(const std::string &prompt, ISBN &isbn,
                      std::string &title);
    bool GetSpareISBN(const std::string &prompt, ISBN &isbn);
    bool GetSpareCallNum(const std::string &prompt, CallNum &call_num);
    bool GetSpareCopyID(const std::string &prompt, CopyID &copy_id);

 private:
    static void GetValidUser(UserID &user_id, User &user);
};

class ReaderInterface : virtual public Interface
{
 public:
    ReaderInterface() = default;
    virtual ~ReaderInterface() = default;

    virtual void MainMenu(Context *context);
    virtual void BorrowBook(Context *context);
    virtual void RequestBook(Context *context);
    virtual void GetRequested(Context *context);

    static ReaderInterface * Instance();

 protected:
    int PerpareMainMenuAndShowInfo(Context *context);
    void ShowBookInfoFromMain(Context *context, int choice);

 private:
    typedef std::pair<ISBN, CopyID> TempBook;

    void ShowBorrowed();
    void ShowRequested();

    UserID user_id_;
    std::vector<TempBook> borrowed_;
    std::vector<TempBook> requested_;
};

class AdminInterface : virtual public Interface
{
 public:
    AdminInterface() = default;
    virtual ~AdminInterface() = default;

    virtual void MainMenu(Context *context);
    virtual void ShowUser(Context *context);
    virtual void CreateUser(Context *context);
    virtual void RemoveUser(Context *context);

    static AdminInterface * Instance();

 protected:
    virtual void ShowCopiesOfBook(Context *context);
};

class AdminReaderInterface: public AdminInterface, public ReaderInterface
{
 public:
    virtual void MainMenu(Context *context);

    static AdminReaderInterface * Instance();

 protected:
    virtual void ShowCopiesOfBook(Context *context)
    {
        AdminInterface::ShowCopiesOfBook(context);
    }
    virtual void BorrowBook(Context *context)
    {
        ReaderInterface::BorrowBook(context);
    }
    virtual void RequestBook(Context *context)
    {
        ReaderInterface::RequestBook(context);
    }
    virtual void GetRequested(Context *context)
    {
        ReaderInterface::GetRequested(context);
    }

    virtual void ShowUser(Context *context)
    {
        AdminInterface::ShowUser(context);
    }
    virtual void CreateUser(Context *context)
    {
        AdminInterface::CreateUser(context);
    }
    virtual void RemoveUser(Context *context)
    {
        AdminInterface::RemoveUser(context);
    }
};

}  // namespace library_manager

#endif  // INTERFACE_H_
