#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>

namespace library_manager {

class Context;

class Interface
{
 public:
    Interface();
    virtual ~Interface();

    void WelcomeScreen(Context *context);
    virtual void MainMenu(Context *context);
    virtual void Query(Context *context);
    virtual void ShowBook(Context *context);
    virtual void RequestBook(Context *context);

    static Interface * GetInstance();

 protected:
    enum class User { None, Reader, Admin, Guest };
    static std::string ReadLine(const std::string &promt);
    static std::string ReadPassword(const std::string &prompt);
    static User Login(const std::string &user_name,
                      const std::string &password);
    static User GetValidUser();
};

class ReaderInterface : public Interface
{
 public:
    ReaderInterface();
    virtual ~ReaderInterface();

    virtual void MainMenu(Context *context);

    static ReaderInterface * GetInstance();
};

class AdminInterface : public Interface
{
 public:
    AdminInterface();
    virtual ~AdminInterface();

    virtual void MainMenu(Context *context);

    static AdminInterface * GetInstance();
};

class GuestInterface : public Interface
{
 public:
    GuestInterface();
    virtual ~GuestInterface();

    virtual void MainMenu(Context *context);

    static GuestInterface * GetInstance();
};

}  // namespace library_manager

#endif  // INTERFACE_H_
