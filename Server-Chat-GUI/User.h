#pragma once
#include <iostream>
#include <string>
#include <exception>


class User
{
private:
	std::string _name;
	std::string _password;
    bool _banned;
    bool _online;

public:
	//Конструткоры, деструкторы
	User() = default;
	User(const std::string& name, const std::string& password);
	User(const std::string& name);

	User(const User& other);
	User(User&& other) noexcept;
	virtual ~User() = default;

	//Гетеры
	std::string getName() const;
	std::string getPassword() const;
    bool isBanned() const;


	//Сетеры
	void setName(std::string& name);
	void setPassword(std::string& password);
    void setBanned(bool b);

	//Операторы
	User& operator=(User& other);
	User& operator=(User&& other) noexcept;

    bool isOnline() const;
    void setOnline(bool newOnline);
};
