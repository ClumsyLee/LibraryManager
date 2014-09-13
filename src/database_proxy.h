#ifndef DATABASE_PROXY_H_
#define DATABASE_PROXY_H_

#include <memory>
#include <string>
#include "common.h"

namespace sql {

class Connection;  // forward declaration
class ResultSet;

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

    User Login(UserID, const std::string &password);

    // name, phone_num
    QueryResult ReaderInfo(UserID reader_id);

    // [isbn, title, copy_id, due_date, request_num, call_num]
    QueryResult QueryBorrowed(UserID reader_id);
    QueryResult QueryRequested(UserID reader_id);

    // [isbn, title, author, imprint]
    QueryResult QueryByTitle(const std::string &title);
    QueryResult QueryByAuthor(const std::string &author);

    // isbn, title, author, imprint, abstract, table_of_content, call_num
    QueryResult BookInfo(ISBN isbn);

    // [copy_id, position, volume, status, due_date, request_num]
    QueryResult CopiesOfBook(ISBN isbn);


    // Return true if succeed, false otherwise.
    bool BorrowCopy(UserID reader_id, const CopyID &copy_id);
    bool ReturnCopy(const CopyID &copy_id);
    bool RequestCopy(UserID reader_id, const CopyID &copy_id);


    static DatabaseProxy * Instance();

 private:
    std::unique_ptr<sql::Connection> connection_;
};

}  // namespace library_manager

#endif  // DATABASE_PROXY_H_
