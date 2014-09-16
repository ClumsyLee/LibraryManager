#include <vector>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include "database_proxy.h"

namespace {

std::string HashPassword(const std::string &password,
                         const std::string &salt)
{
    return password;
}

std::string GetRandomSalt(std::size_t bytes)
{
    return std::string();
}


}  // namespace


namespace library_manager {

DatabaseProxy::DatabaseProxy()
        : connection_(get_driver_instance()->connect("localhost",
                                                     "thomas",
                                                     ""))
{
    connection_->setSchema("library");
}

void DatabaseProxy::ResetConnection(const std::string &host,
                                    const std::string &user_name,
                                    const std::string &password)
{
    connection_.reset(
        get_driver_instance()->connect(host, user_name, password));
}

bool DatabaseProxy::Login(UserID user_id, const std::string &password,
                          User &user)
{
    static Statement get_salt(connection_->prepareStatement(
        "SELECT salt FROM User WHERE id=?"));
    static Statement login(connection_->prepareStatement(
        "SELECT is_admin FROM User WHERE id=? AND password=?"));

    // get salt
    get_salt->setUInt(1, user_id);
    QueryResult salt_result(get_salt->executeQuery());
    if (!salt_result->next())  // user not found
        return false;

    // query
    login->setUInt(1, user_id);
    login->setString(2, HashPassword(password, salt_result->getString(1)));
    QueryResult login_result(login->executeQuery());
    if (!login_result->next())  // login failed
    {
        return false;
    }
    else
    {
        if (login_result->getBoolean(1))  // is admin
            user = User::ADMIN;
        else
            user = User::READER;
        return true;
    }
}

DatabaseProxy::QueryResult DatabaseProxy::ReaderInfo(UserID reader_id)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT name FROM User WHERE id=?"));

    statement->setUInt(1, reader_id);
    return QueryResult(statement->executeQuery());
}

DatabaseProxy::QueryResult DatabaseProxy::QueryBorrowed(UserID reader_id)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT CopyInfo.* FROM Borrow JOIN CopyInfo "
        "ON Borrow.copy_id=CopyInfo.id "
        "WHERE Borrow.reader_id=?"));

    statement->setUInt(1, reader_id);
    return QueryResult(statement->executeQuery());
}

DatabaseProxy::QueryResult DatabaseProxy::QueryRequested(UserID reader_id)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT CopyInfo.* FROM Request JOIN CopyInfo "
        "ON Request.copy_id=CopyInfo.id "
        "WHERE Request.reader_id=?"));

    statement->setUInt(1, reader_id);
    return QueryResult(statement->executeQuery());
}

DatabaseProxy::QueryResult DatabaseProxy::QueryByTitle(const std::string &title)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT isbn, title, author, imprint FROM Book WHERE title LIKE ?"));

    statement->setString(1, title);
    return QueryResult(statement->executeQuery());
}

DatabaseProxy::QueryResult DatabaseProxy::QueryByAuthor(
        const std::string &author)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT isbn, title, author, imprint FROM Book WHERE author LIKE ?"));

    statement->setString(1, author);
    return QueryResult(statement->executeQuery());
}

DatabaseProxy::QueryResult DatabaseProxy::BookInfo(ISBN isbn)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT * FROM Book WHERE isbn=?"));

    statement->setUInt64(1, isbn);
    return QueryResult(statement->executeQuery());
}

DatabaseProxy::QueryResult DatabaseProxy::CopiesOfBook(ISBN isbn)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT id, status, due_date, request_num FROM CopyInfo "
        "WHERE isbn=?"));

    statement->setUInt64(1, isbn);
    return QueryResult(statement->executeQuery());
}

// bool DatabaseProxy::BorrowCopy(UserID reader_id, const CopyID &copy_id)
// {
//     static Statement statement(connection_->prepareStatement(
//         ""));

// }

// bool DatabaseProxy::ReturnCopy(const CopyID &copy_id)
// {

// }

// bool DatabaseProxy::RequestCopy(UserID reader_id, const CopyID &copy_id)
// {

// }




DatabaseProxy * DatabaseProxy::Instance()
{
    static DatabaseProxy proxy;
    return &proxy;
}




}  // namespace library_manager
