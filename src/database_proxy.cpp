#include <vector>

#include <openssl/evp.h>
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

// std::string GetRandomSalt(std::size_t bytes)
// {
//     return std::string();
// }


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

DatabaseProxy::QueryResult DatabaseProxy::Query(const std::string &keyword)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT isbn, title, author, imprint FROM Book "
        "WHERE title LIKE ? OR author LIKE ?"));

    statement->setString(1, keyword);
    statement->setString(2, keyword);
    return QueryResult(statement->executeQuery());
}

DatabaseProxy::QueryResult DatabaseProxy::BookInfo(ISBN isbn)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT * FROM Book WHERE isbn=?"));

    statement->setUInt64(1, isbn);
    return QueryResult(statement->executeQuery());
}

DatabaseProxy::QueryResult DatabaseProxy::CopyInfo(const CopyID &copy_id)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT * FROM CopyInfo "
        "WHERE id=?"));

    statement->setString(1, copy_id);
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

bool DatabaseProxy::BorrowCopy(UserID reader_id, const CopyID &copy_id)
{
    auto copy_info = CopyInfo(copy_id);
    if (!copy_info->next())
    {
        std::cerr << "找不到条形码为 " << copy_id << " 的副本\n";
        return false;
    }
    if (copy_info->getString("status") != "ON_SHELF")
    {
        std::cerr << "条形码为 " << copy_id
                  << " 的副本当前不在架上, 不能借出\n";
        return false;
    }

    InsertBorrow(reader_id, copy_id, kLoanPeriod);
    UpdateCopyStatus(copy_id, "LENT");
    return true;
}

bool DatabaseProxy::ReturnCopy(const CopyID &copy_id)
{
    auto copy_info = CopyInfo(copy_id);
    if (!copy_info->next())
    {
        std::cerr << "找不到条形码为 " << copy_id << " 的副本\n";
        return false;
    }
    if (copy_info->getString("status") != "LENT")
    {
        std::cerr << "条形码为 " << copy_id
                  << " 的副本当前不在借出状态, 不能归还\n";
        return false;
    }

    DeleteBorrow(copy_id);

    if (copy_info->getInt("request_num") != 0)
        UpdateCopyStatus(copy_id, "AT_LOAN_DESK");
    else
        UpdateCopyStatus(copy_id, "ON_SHELF");

    return true;
}

bool DatabaseProxy::GetRequested(UserID reader_id, const CopyID &copy_id)
{
    static Statement requests_now(connection_->prepareStatement(
        "SELECT reader_id FROM Request WHERE copy_id=? ORDER BY time"));

    auto copy_info = CopyInfo(copy_id);
    if (!copy_info->next())
    {
        std::cerr << "找不到条形码为 " << copy_id << " 的副本\n";
        return false;
    }
    if (copy_info->getString("status") != "AT_LOAN_DESK")
    {
        std::cerr << "条形码为 " << copy_id
                  << " 的副本当前不在预约架上, 不能取书\n";
        return false;
    }

    requests_now->setString(1, copy_id);
    QueryResult request_status(requests_now->executeQuery());
    if (request_status->next() &&
        request_status->getUInt("reader_id") == reader_id)
    {
        // success
        DeleteRequest(reader_id, copy_id);
        InsertBorrow(reader_id, copy_id, kLoanPeriod);
        UpdateCopyStatus(copy_id, "LENT");
        return true;
    }
    else
    {
        std::cerr << "该读者不是当前可以取书的预约者\n";
        return false;
    }
}

// bool DatabaseProxy::RequestCopy(UserID reader_id, const CopyID &copy_id)
// {

// }

void DatabaseProxy::UpdateCopyStatus(const CopyID &copy_id,
                                     const std::string &status)
{
    static Statement update(connection_->prepareStatement(
        "UPDATE Copy SET status=? WHERE id=?"));

    update->setString(1, status);
    update->setString(2, copy_id);
    update->execute();
}

void DatabaseProxy::InsertBorrow(UserID reader_id, const CopyID &copy_id,
                                 int days)
{
    static Statement borrow(connection_->prepareStatement(
        "INSERT INTO Borrow VALUES (?, ?, CURRENT_DATE(), "
        "DATE_ADD(CURRENT_DATE(), INTERVAL ? DAY), 0, 0)"));

    borrow->setUInt(1, reader_id);
    borrow->setString(2, copy_id);
    borrow->setInt(3, days);
    borrow->execute();
}

void DatabaseProxy::DeleteBorrow(const CopyID &copy_id)
{
    static Statement delete_borrow(connection_->prepareStatement(
        "DELETE FROM Borrow WHERE copy_id=?"));

    delete_borrow->setString(1, copy_id);
    delete_borrow->execute();
}

void DatabaseProxy::DeleteRequest(UserID reader_id, const CopyID &copy_id)
{
    static Statement delete_request(connection_->prepareStatement(
        "DELETE FROM Request WHERE copy_id=? AND reader_id=?"));

    delete_request->setString(1, copy_id);
    delete_request->setUInt(2, reader_id);
    delete_request->execute();
}



DatabaseProxy * DatabaseProxy::Instance()
{
    static DatabaseProxy proxy;
    return &proxy;
}




}  // namespace library_manager
