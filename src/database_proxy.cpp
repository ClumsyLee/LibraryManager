#include "mysql_connection.h"
#include <cppconn/driver.h>
#include "database_proxy.h"

namespace library_manager {

DatabaseProxy::DatabaseProxy()
        : connection_(get_driver_instance()->connect("localhost",
                                                     "thomas",
                                                     ""))
{}

void DatabaseProxy::ResetConnection(const std::string &host,
                                    const std::string &user_name,
                                    const std::string &password)
{
    connection_.reset(
        get_driver_instance()->connect(host, user_name, password));
}


DatabaseProxy * DatabaseProxy::Instance()
{
    static DatabaseProxy proxy;
    return &proxy;
}



}  // namespace library_manager
