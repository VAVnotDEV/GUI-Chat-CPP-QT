#include "database.h"
#include <iostream>


Database::Database() : _chatDB(nullptr)
{
    int result = sqlite3_open("chat.db", &_chatDB);
    if (result != SQLITE_OK)
    {
        std::cout << "[DB] Open error: " << sqlite3_errmsg(_chatDB) << "\n";
        sqlite3_close(_chatDB);
        _chatDB = nullptr;
        return;
    }

    std::cout << "[DB] Open OK\n";

    const char* sql_users =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "login TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "banned INTEGER DEFAULT 0,"
        "online INTEGER DEFAULT 0"
        ");";

    const char* sql_messages =
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "from_login TEXT NOT NULL,"
        "to_login TEXT NOT NULL,"
        "text TEXT NOT NULL"
        ");";

    char* err = nullptr;

    if (sqlite3_exec(_chatDB, sql_users, nullptr, nullptr, &err) != SQLITE_OK)
    {
        std::cout << "[DB] Create users failed: " << err << "\n";
        sqlite3_free(err);
    }

    const char* sql_common =
        "INSERT OR IGNORE INTO users (login, password, banned, online) "
        "VALUES ('common', 'common', 0, 0);";

    if (sqlite3_exec(_chatDB, sql_common, nullptr, nullptr, &err) != SQLITE_OK)
    {
        std::cout << "[DB] Insert common failed: " << err << "\n";
        sqlite3_free(err);
    }


    if (sqlite3_exec(_chatDB, sql_messages, nullptr, nullptr, &err) != SQLITE_OK)
    {
        std::cout << "[DB] Create messages failed: " << err << "\n";
        sqlite3_free(err);
    }
}

Database::~Database()
{
    if (_chatDB) {
        sqlite3_close(_chatDB);
        _chatDB = nullptr;
    }
}

bool Database::isOpen() const
{
    return _chatDB;
}

bool Database::addUser(const std::string &login, const std::string& password)
{
    if (!_chatDB) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO users (login, password) VALUES (?, ?)";

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return ok;

}

bool Database::userExist(const std::string &login) const
{
    if (!_chatDB) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT 1 FROM users WHERE login = ? LIMIT 1";

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);

    sqlite3_finalize(stmt);
    return exists;

}

bool Database::getAllUsers(std::vector<std::string> &outLogins) const
{
    if (!_chatDB) return false;

    const char* sql = "SELECT login FROM users ORDER BY login ASC";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (login)
            outLogins.emplace_back(login);
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::checkPassword(const std::string &login, const std::string password) const
{
    if (!_chatDB) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT password FROM users WHERE login = ?";

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return false; // пользователя нет
    }

    const unsigned char* dbPassU = sqlite3_column_text(stmt, 0);
    std::string dbPass = dbPassU ? reinterpret_cast<const char*>(dbPassU) : "";

    sqlite3_finalize(stmt);

    return dbPass == password;

}

bool Database::setBanned(const std::string &login, bool banned)
{
    if (!_chatDB) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE users SET banned = ? WHERE login = ?";

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, banned ? 1 : 0);
    sqlite3_bind_text(stmt, 2, login.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return ok;

}

bool Database::isBanned(const std::string &login) const
{
    if (!_chatDB) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT banned FROM users WHERE login = ?";

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return false;
    }

    int banned = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    return banned != 0;

}

bool Database::setOnline(const std::string &login, bool online)
{
    if (!_chatDB) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE users SET online = ? WHERE login = ?";

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, online ? 1 : 0);
    sqlite3_bind_text(stmt, 2, login.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::isOnline(const std::string &login) const
{
    if (!_chatDB) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT online FROM users WHERE login = ?";

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

    bool online = false;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        online = sqlite3_column_int(stmt, 0) != 0;

    sqlite3_finalize(stmt);
    return online;
}

bool Database::addMessage(const std::string &from, const std::string &text, const std::string &to)
{
    if (!_chatDB) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "INSERT INTO messages (from_login, text, to_login) "
        "VALUES (?, ?, ?)";

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, from.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, text.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, to.c_str(),   -1, SQLITE_TRANSIENT);


    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return ok;
}

bool Database::getMessagesForUser(const std::string &login, std::vector<Message<std::string> > &out)
{
    if (!_chatDB) return false;

    const char* sql =
        "SELECT from_login, to_login, text "
        "FROM messages "
        "WHERE to_login = ? "
        "   OR from_login = ? "
        "   OR to_login = 'common' "
        "ORDER BY id ASC";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, login.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* from = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* to   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        out.emplace_back(from, to, text);
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::getAllMessages(std::vector<Message<std::string>> &out)
{
    if (!_chatDB) return false;

    const char* sql =
        "SELECT from_login, to_login, text "
        "FROM messages "
        "ORDER BY id ASC";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(_chatDB, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* from = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* to   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        out.emplace_back(from, to, text);
    }

    sqlite3_finalize(stmt);
    return true;
}
