#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include "sqlite3.h"
#include "Message.h"
#include <vector>

class Database
{
public:
    Database();
    ~Database();

    bool isOpen() const;
    bool addUser(const std::string& login, const std::string& password);
    bool userExist(const std::string& login) const;
    bool getAllUsers(std::vector<std::string>& outLogins) const;
    bool checkPassword(const std::string& login, const std::string password) const;
    bool setBanned(const std::string& login, bool banned);
    bool isBanned(const std::string& login) const;
    bool setOnline(const std::string& login, bool online);
    bool isOnline(const std::string& login) const;
    bool addMessage(const std::string& from,const std::string& text, const std::string& to);
    bool getMessagesForUser(const std::string& login, std::vector<Message<std::string>>& out);
    bool getAllMessages(std::vector<Message<std::string>>& out);
private:
    sqlite3* _chatDB;
};

#endif // DATABASE_H
