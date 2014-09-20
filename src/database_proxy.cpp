#include <fstream>
#include <sstream>
#include <vector>

#include <openssl/evp.h>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include "database_proxy.h"

namespace {

const int kIterations = 100000;
const int kKeyLen = 64;

typedef std::basic_string<unsigned char> BinString;

BinString Hex2Bin(const std::string &hex)
{
    BinString bin;

    std::istringstream iss;
    iss >> std::hex;

    int size = hex.size();
    for (int i = 0; i < size; i += 2)
    {
        iss.clear();
        iss.str(hex.substr(i, 2));
        unsigned num;
        iss >> num;
        bin.push_back(num);
    }
    return bin;
}

std::string Bin2Hex(const BinString &bin)
{
    std::ostringstream oss;
    oss << std::hex;
    oss.fill('0');
    for (unsigned num : bin)
    {
        oss.width(2);
        oss << num;
    }
    return oss.str();
}

int PBKDF2_HMAC_SHA_512(const std::string &password, const BinString &salt,
                        int iterations, int key_len, BinString &out)
{
    out.resize(key_len);
    return PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                             salt.data(), salt.size(),
                             iterations, EVP_sha512(), key_len, &out[0]);
}

std::string HashPassword(const std::string &password, const BinString &salt)
{
    BinString hashed_password;
    hashed_password.resize(kKeyLen);
    if (!PBKDF2_HMAC_SHA_512(password, salt, kIterations, kKeyLen,
                             hashed_password))
        throw std::runtime_error("PBKDF2_HMAC_SHA_512 failed\n");

    return Bin2Hex(hashed_password);
}

BinString GetRandomSalt(std::size_t bytes)
{
    BinString random_bytes;
    random_bytes.resize(bytes);
    std::ifstream fin("/dev/random");
    if (!fin.is_open())
        throw std::runtime_error("Cannot open /dev/random");

    fin.read(reinterpret_cast<char *>(&random_bytes[0]), bytes);
    return std::move(random_bytes);
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
        "SELECT admin_priv, reader_priv FROM User WHERE id=? AND password=?"));

    // get salt
    get_salt->setUInt(1, user_id);
    QueryResult salt_result(get_salt->executeQuery());
    if (!salt_result->next())  // user not found
        return false;

    // query
    login->setUInt(1, user_id);
    login->setString(2, HashPassword(password,
                                     Hex2Bin(salt_result->getString(1))));
    QueryResult login_result(login->executeQuery());
    if (!login_result->next())  // login failed
    {
        return false;
    }
    else
    {
        if (login_result->getBoolean("admin_priv"))  // is admin
        {
            if (login_result->getBoolean("reader_priv"))
                user = User::ADMIN_READER;
            else
                user = User::ADMIN;
        }
        else
            user = User::READER;
        return true;
    }
}

bool DatabaseProxy::ChangePassword(UserID user_id,
                                   const std::string &old_password,
                                   const std::string &new_password)
{
    static Statement change_password(connection_->prepareStatement(
        "UPDATE User SET salt=?, password=? WHERE id=?"));

    User user;
    if (!Login(user_id, old_password, user))
    {
        std::cout << "原密码错误\n";
        return false;
    }
    change_password->setUInt(3, user_id);

    BinString salt(GetRandomSalt(kKeyLen));
    change_password->setString(1, Bin2Hex(salt));
    change_password->setString(2, HashPassword(new_password, salt));

    change_password->execute();
    return true;
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

DatabaseProxy::QueryResult DatabaseProxy::BookInfo(const CallNum &call_num)
{
    static Statement statement(connection_->prepareStatement(
        "SELECT * FROM Book WHERE call_num=?"));

    statement->setString(1, call_num);
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
    if (!ReaderInfo(reader_id)->next())
    {
        std::cerr << "不存在ID为 " << reader_id << "的读者\n";
        return false;
    }

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

bool DatabaseProxy::AbleToGetRquested(UserID reader_id, const CopyID &copy_id)
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
        return false;
    }

    requests_now->setString(1, copy_id);
    QueryResult request_status(requests_now->executeQuery());
    if (request_status->next() &&
        request_status->getUInt("reader_id") == reader_id)
        return true;
    else
        return false;
}

bool DatabaseProxy::GetRequested(UserID reader_id, const CopyID &copy_id)
{
    if (AbleToGetRquested(reader_id, copy_id))
    {
        // success
        DeleteRequest(reader_id, copy_id);
        InsertBorrow(reader_id, copy_id, kLoanPeriod);
        UpdateCopyStatus(copy_id, "LENT");
        return true;
    }
    else
    {
        return false;
    }
}

bool DatabaseProxy::RequestCopy(UserID reader_id, const CopyID &copy_id)
{
    if (!ReaderInfo(reader_id)->next())
    {
        std::cerr << "不存在ID为 " << reader_id << "的读者\n";
        return false;
    }

    auto copy_info = CopyInfo(copy_id);
    if (!copy_info->next())
    {
        std::cerr << "找不到条形码为 " << copy_id << " 的副本\n";
        return false;
    }
    if (copy_info->getString("status") != "LENT")
        return false;

    InsertRequest(reader_id, copy_id);
    return true;
}

bool DatabaseProxy::CreateUser(User type, UserID user_id,
                               const std::string &name,
                               const std::string &password)
{
    if (ReaderInfo(user_id)->next())
    {
        std::cerr << "已存在ID为 " << user_id << " 的用户\n";
        return false;
    }

    BinString salt = GetRandomSalt(kKeyLen);
    InsertUser(type, user_id, name, HashPassword(password, salt),
               Bin2Hex(salt));
    return true;
}

bool DatabaseProxy::RemoveUser(UserID user_id)
{
    if (!ReaderInfo(user_id)->next())
    {
        std::cerr << "不存在ID为 " << user_id << " 的用户\n";
        return false;
    }
    if (QueryBorrowed(user_id)->next())
    {
        std::cerr << "该用户有未归还的书籍, 不能删除\n";
        return false;
    }

    DeleteRequestOfUser(user_id);
    DeleteUser(user_id);
    return true;
}

bool DatabaseProxy::CreateBook(ISBN isbn, const CallNum &call_num,
                               const std::string &title,
                               const std::string &author,
                               const std::string &imprint)
{
    if (BookInfo(isbn)->next())
    {
        std::cerr << "已经存在ISBN为 " << isbn << "的书籍\n";
        return false;
    }
    if (BookInfo(call_num)->next())
    {
        std::cerr << "已经存在索书号为 " << call_num << "的书籍\n";
        return false;
    }
    InsertBook(isbn, call_num, title, author, imprint);
    return true;
}

bool DatabaseProxy::CreateCopy(const CopyID &copy_id, ISBN isbn)
{
    if (CopyInfo(copy_id)->next())
    {
        std::cerr << "已经存在条形码为 " << copy_id << "的副本\n";
        return false;
    }
    if (!BookInfo(isbn)->next())
    {
        std::cerr << "不存在ISBN为 " << isbn << "的书籍\n";
        return false;
    }
    InsertCopy(copy_id, isbn);
    return true;
}


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

void DatabaseProxy::InsertRequest(UserID reader_id, const CopyID &copy_id)
{
    static Statement request(connection_->prepareStatement(
        "INSERT INTO Request VALUES (?, ?, NOW())"));

    request->setUInt(1, reader_id);
    request->setString(2, copy_id);
    request->execute();
}

void DatabaseProxy::InsertUser(User type, UserID user_id,
                               const std::string &name,
                               const std::string &password,
                               const std::string &salt)
{
    static Statement insert_user(connection_->prepareStatement(
        "INSERT INTO User VALUES (?, ?, ?, ?, ?, ?)"));

    insert_user->setUInt(1, user_id);
    insert_user->setString(2, name);
    insert_user->setString(3, password);
    insert_user->setString(4, salt);

    insert_user->setBoolean(5 , (type == User::ADMIN ||
                                 type == User::ADMIN_READER));
    insert_user->setBoolean(6, (type == User::READER ||
                                type == User::ADMIN_READER));

    insert_user->execute();
}

void DatabaseProxy::InsertBook(ISBN isbn, const CallNum &call_num,
                               const std::string &title,
                               const std::string &author,
                               const std::string &imprint)
{
    static Statement insert_book(connection_->prepareStatement(
        "INSERT INTO Book VALUES (?, ?, ?, ?, ?)"));

    insert_book->setUInt64(1, isbn);
    insert_book->setString(2, call_num);
    insert_book->setString(3, title);
    insert_book->setString(4, author);
    insert_book->setString(5, imprint);

    insert_book->execute();
}

void DatabaseProxy::InsertCopy(const CopyID &copy_id, ISBN isbn)
{
    static Statement insert_copy(connection_->prepareStatement(
        "INSERT INTO Copy(id, isbn, status) VALUES (?, ?, ON_SHELF)"));

    insert_copy->setString(1, copy_id);
    insert_copy->setUInt64(2, isbn);

    insert_copy->execute();
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

void DatabaseProxy::DeleteRequestOfUser(UserID reader_id)
{
    static Statement delete_request(connection_->prepareStatement(
        "DELETE FROM Request WHERE reader_id=?"));

    delete_request->setUInt(1, reader_id);
    delete_request->execute();
}

void DatabaseProxy::DeleteRequestOfCopy(const CopyID &copy_id)
{
    static Statement delete_request(connection_->prepareStatement(
        "DELETE FROM Request WHERE copy_id=?"));

    delete_request->setString(1, copy_id);
    delete_request->execute();
}

void DatabaseProxy::DeleteUser(UserID user_id)
{
    static Statement delete_user(connection_->prepareStatement(
        "DELETE FROM User WHERE id=?"));

    delete_user->setUInt(1, user_id);
    delete_user->execute();
}


DatabaseProxy * DatabaseProxy::Instance()
{
    static DatabaseProxy proxy;
    return &proxy;
}




}  // namespace library_manager
