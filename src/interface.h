#ifndef INTERFACE_H_
#define INTERFACE_H_

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
};

class ReaderInterface
{
 public:
    ReaderInterface();
    virtual ~ReaderInterface();

    virtual void MainMenu(Context *context);

    static ReaderInterface * GetInstance();
};

class AdminInterface
{
 public:
    AdminInterface();
    virtual ~AdminInterface();

    virtual void MainMenu(Context *context);

    static AdminInterface * GetInstance();
};

class GuestInterface
{
 public:
    GuestInterface();
    virtual ~GuestInterface();

    virtual void MainMenu(Context *context);

    static GuestInterface * GetInstance();
};

}  // namespace library_manager

#endif  // INTERFACE_H_
