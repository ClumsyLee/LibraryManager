#ifndef DATABASE_PROXY_H_
#define DATABASE_PROXY_H_

#include <memory>
#include <string>
#include "common.h"

namespace sql {

class Connection;  // forward declaration
class ResultSet;
class PreparedStatement;

}  // namespace sql

namespace library_manager {

class DatabaseProxy
{
 public:
    typedef std::unique_ptr<sql::ResultSet> QueryResult;

    DatabaseProxy();

    void ResetConnection(const std::string &host,
                         const std::string &user_name,
                         const std::string &password);

    bool Login(UserID user_id, const std::string &password, User &user);

    // name
    QueryResult ReaderInfo(UserID reader_id);

    // [isbn, title, id, due_date, request_num, call_num]
    QueryResult QueryBorrowed(UserID reader_id);
    QueryResult QueryRequested(UserID reader_id);

    // [isbn, title, author, imprint]
    QueryResult Query(const std::string &keyword);

    // isbn, title, author, imprint, abstract, table_of_contents, call_num
    QueryResult BookInfo(ISBN isbn);

    // id, title, isbn, call_num, status, due_date, request_num
    QueryResult CopyInfo(const CopyID &copy_id);

    // [id, status, due_date, request_num]
    QueryResult CopiesOfBook(ISBN isbn);


    // Return true if succeed, false otherwise.
    bool BorrowCopy(UserID reader_id, const CopyID &copy_id);
    bool GetRequested(UserID reader_id, const CopyID &copy_id);
    bool ReturnCopy(const CopyID &copy_id);
    bool RequestCopy(UserID reader_id, const CopyID &copy_id);


    static DatabaseProxy * Instance();

 private:
    void UpdateCopyStatus(const CopyID &copy_id, const std::string &status);
    void InsertBorrow(UserID reader_id, const CopyID &copy_id, int days);
    void DeleteBorrow(const CopyID &copy_id);
    void DeleteRequest(UserID reader_id, const CopyID &copy_id);

    typedef std::unique_ptr<sql::PreparedStatement> Statement;
    std::unique_ptr<sql::Connection> connection_;
};

}  // namespace library_manager

#endif  // DATABASE_PROXY_H_
