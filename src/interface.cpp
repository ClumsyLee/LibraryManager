#include <iostream>
#include <boost/lexical_cast.hpp>
#include "interface.h"
#include "context.h"

namespace library_manager {

void Interface::WelcomeScreen(Context *context)
{
    std::cout << "欢迎来到图书馆管理系统\n";
    User user = GetValidUser();

    switch (user)
    {
        case User::Reader:
        {
            context->SetInterface(ReaderInterface::GetInstance());
            break;
        }
        case User::Admin:
        {
            context->SetInterface(AdminInterface::GetInstance());
            break;
        }
        case User::Guest:
        {
            context->SetInterface(GuestInterface::GetInstance());
            break;
        }
        default:
        {
            throw std::runtime_error(
                "Bad return value from GetValidUser: " +
                boost::lexical_cast<std::string>(user));
        }
    }
    // Login successful, entering main menu.
    context->SetState(&Interface::MainMenu);
}





}  // namespace library_manager
