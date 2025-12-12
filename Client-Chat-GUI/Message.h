#pragma once
#include "User.h"


template <class T>
class Message
{
private:
	std::string _from;
	std::string _to;
	T _message;

public:
	Message() = default;
	Message(std::string from, std::string to, T message) : _from(from), _to(to), _message(message) {}
	Message(const Message& other) : _from(other._from), _to(other._to), _message(other._message) {}
	Message(const Message&& other) noexcept : _from(move(other._from)), _to(move(other._to)), _message(move(other._message)) {}
	~Message() = default;

	std::string getTo()  const { return _to; }
	std::string getFrom() const { return _from; }
	T getMessage() const { return _message; }

	void setTo(std::string value) { _to = value; }
	void setFrom(std::string& value) { _from = value; }
	void setMessage(T& value) { _message = value; }

	Message& operator=(const Message& other)
	{
		_from = other._from;
		_to = other._to;
		_message = other._message;

		return *this;
	}
	
	Message& operator=(Message&& other) noexcept
	{
		_from = move(other._from);
		_to = move(other._to);
		_message = move(other._message);

		return *this;
	}

};