#ifndef MANAGER_H_
#define MANAGER_H_

namespace library_manager {

class Manager
{
 public:
    Manager();
    int Run();

 private:
    virtual void MainMenu();
    virtual void Query() = 0;
    virtual void ShowBook() = 0;
    virtual void RequestBook() = 0;
};

class ReaderManager
{
 public:
    ReaderManager();

 private:
    virtual void MainMenu();
};

class AdminManager
{
 public:
    AdminManager();

 private:
    virtual void MainMenu();

};

class GuestManager
{
 public:
    GuestManager();

 private:
    virtual void MainMenu();

};

}

#endif  // MANAGER_H_
