#include "User.h"


//Конструкторы
bool User::isOnline() const
{
    return _online;
}

void User::setOnline(bool newOnline)
{
    _online = newOnline;
}

User::User(const std::string& name,const std::string& password) : _name(name), _password(password), _online(false), _banned(false) {};
User::User(const std::string& name) : _name(name), _banned(false), _online(false) {};
User::User(const User& other) : _name(other._name), _password(other._password), _online(other._online), _banned(other._banned){}
User::User(User&& other) noexcept : _name(move(other._name)), _password(move(other._password)), _online(other._online), _banned(other._banned) {}

//Гетеры
std::string User::getName() const { return _name; }
std::string User::getPassword() const { return _password; }

bool User::isBanned() const {return _banned;}

//Сетеры
void User::setName(std::string& name) { _name = name; }
void User::setPassword(std::string& password) { _password = password; }

void User::setBanned(bool b) {_banned = b; }

//Операторы
User& User::operator=(User& other)
{
	_name = other._name;
	_password = other._password;
    _online = other._online;
    _banned = other._banned;

	return *this;
}
User& User::operator=(User&& other) noexcept
{
	_name = move(other._name);
	_password = move(other._password);
    _online = other._online;
    _banned = other._banned;

	return *this;
}

