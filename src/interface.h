#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>
#include <vector>
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
    virtual void MainMenu(Context *context);
    virtual void Query(Context *context);
    virtual void ShowBook(Context *context);
    virtual void RequestBook(Context *context);

    UserID user_id() const { return user_id_; }
    void set_user_id(UserID user_id) { user_id_ = user_id; }

    static Interface * Instance();

 protected:
    static std::string ReadLine(const std::string &promt);
    static UserID ReadUserID(const std::string &promt);
    static std::string ReadPassword(const std::string &prompt);

 private:
    static User GetValidUser();

    UserID user_id_;
};

class ReaderInterface : public Interface
{
 public:
    ReaderInterface() = default;
    virtual ~ReaderInterface() = default;

    virtual void MainMenu(Context *context);

    static ReaderInterface * Instance();

 private:
    void ShowReaderInfo() const;
    void ShowBorrowed();
    void ShowRequested();

    std::vector<ISBN> borrowed_;
    std::vector<CopyID> requested_;
};

class AdminInterface : public Interface
{
 public:
    AdminInterface() = default;
    virtual ~AdminInterface() = default;

    virtual void MainMenu(Context *context);

    static AdminInterface * Instance();
};

class GuestInterface : public Interface
{
 public:
    GuestInterface();
    virtual ~GuestInterface();

    virtual void MainMenu(Context *context);

    static GuestInterface * Instance();
};

}  // namespace library_manager

#endif  // INTERFACE_H_
