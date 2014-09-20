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
    bool ChangePassword(UserID user_id,
                        const std::string &old_password,
                        const std::string &new_password);

    // name
    QueryResult ReaderInfo(UserID reader_id);

    // [isbn, title, id, due_date, request_num, call_num]
    QueryResult QueryBorrowed(UserID reader_id);
    QueryResult QueryRequested(UserID reader_id);

    // [isbn, title, author, imprint]
    QueryResult Query(const std::string &keyword);

    // isbn, title, author, imprint, call_num
    QueryResult BookInfo(ISBN isbn);
    QueryResult BookInfo(const CallNum &call_num);

    // id, title, isbn, call_num, status, due_date, request_num
    QueryResult CopyInfo(const CopyID &copy_id);

    // [id, status, due_date, request_num]
    QueryResult CopiesOfBook(ISBN isbn);

    QueryResult RequestList(const CopyID &copy_id);


    bool Borrowed(UserID reader_id, const CopyID &copy_id);
    bool Requested(UserID reader_id, const CopyID &copy_id);

    // Return true if succeed, false otherwise.
    bool BorrowCopy(UserID reader_id, const CopyID &copy_id);
    bool AbleToGetRquested(UserID reader_id, const CopyID &copy_id);
    bool GetRequested(UserID reader_id, const CopyID &copy_id);
    bool ReturnCopy(const CopyID &copy_id);
    bool RequestCopy(UserID reader_id, const CopyID &copy_id);

    bool CreateUser(User type, UserID id, const std::string &name,
                    const std::string &password);
    bool CreateBook(ISBN isbn, const CallNum &call_num,
                    const std::string &title,
                    const std::string &author, const std::string &imprint);
    bool CreateCopy(const CopyID &copy_id, ISBN isbn);

    bool RemoveUser(UserID user_id);
    bool RemoveBook(ISBN isbn);
    bool RemoveCopy(const CopyID &copy_id);

    bool Lost(const CopyID &copy_id);

    static DatabaseProxy * Instance();

 private:
    void UpdateCopyStatus(const CopyID &copy_id, const std::string &status);
    void InsertBorrow(UserID reader_id, const CopyID &copy_id, int days);
    void InsertRequest(UserID reader_id, const CopyID &copy_id);
    void InsertUser(User type, UserID user_id, const std::string &name,
                    const std::string &password,
                    const std::string &salt);
    void InsertBook(ISBN isbn, const CallNum &call_num,
                    const std::string &title,
                    const std::string &author, const std::string &imprint);
    void InsertCopy(const CopyID &copy_id, ISBN isbn);
    void DeleteBorrow(const CopyID &copy_id);
    void DeleteRequest(UserID reader_id, const CopyID &copy_id);
    void DeleteRequestOfUser(UserID reader_id);
    void DeleteRequest(const CopyID &copy_id);
    void DeleteUser(UserID user_id);
    void DeleteBook(ISBN isbn);
    void DeleteCopy(const CopyID &copy_id);

    typedef std::unique_ptr<sql::PreparedStatement> Statement;
    std::unique_ptr<sql::Connection> connection_;
};

}  // namespace library_manager

#endif  // DATABASE_PROXY_H_
