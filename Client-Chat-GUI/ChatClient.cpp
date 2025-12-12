#include "ChatClient.h"
#include <limits>


//Constructors
ChatClient::ChatClient() : _socket_file_descriptor(-1), _connection(-1), _bytes_read(-1) { }

ChatClient::~ChatClient()
{
#ifdef _WIN32
	if (_socket_file_descriptor != INVALID_SOCKET)
	{
		closesocket(_socket_file_descriptor);
		_socket_file_descriptor = INVALID_SOCKET;
#else
	if (_socket_file_descriptor >= 0)
	{
		close(_socket_file_descriptor);
		_socket_file_descriptor = -1;
#endif
		_connection = -1;
		_bytes_read = -1;
	}
}
//Socket
bool ChatClient::SetupConnection()
{
	_socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

	if(_socket_file_descriptor == -1)
	{
		perror("Socket creation failed failed");  
		return false;
	}

	//_serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	#ifdef _WIN32
	InetPton(AF_INET, L"127.0.0.1", &_serveraddress.sin_addr);
	#else
	inet_pton(AF_INET, "127.0.0.1", &_serveraddress.sin_addr);
	#endif
	_serveraddress.sin_port = htons(PORT);
	_serveraddress.sin_family = AF_INET;

	_connection = connect(_socket_file_descriptor, (struct sockaddr*)&_serveraddress, sizeof(_serveraddress));
	
	if(_connection == -1)
	{
		perror("Connection with the server failed.!");
		return false;
	}
	return true;
}

bool ChatClient::sendData()
{
    char buf[MESSAGE_LENGTH];
    _dep.outputDataHandler(buf);
    int n = send(_socket_file_descriptor, buf, sizeof(buf), 0);
    if(n < 0)
        return false;

    return true;
}

bool ChatClient::recvData()
{
	char buf[MESSAGE_LENGTH];
	int n = recv(_socket_file_descriptor, buf, sizeof(buf),0);
	if (n <= 0) 
	{
		std::cout << "Recv failed\n";
		return false;
	}
	_dep.DataClear();
	_dep.inputDataHandler(buf, n);
	
	return true;
}
//Chat logic
bool ChatClient::authUser(const std::string& login, const std::string& password)
{
	_dep.cmd = _currentCMD = "AUTH_USER";
    _dep.login = login;
    _dep.password = password;

    sendData();
	recvData();

    if(_dep.log == "1")
    {
        _currentUser = login;
		return true;
    }
	return false;
}

bool ChatClient::userRegister(const std::string& login, const std::string& password)
{
	_dep.cmd = "ADD_USER";
    _dep.login = _currentUser = login;
    _dep.password = password;

	sendData();
	recvData();

    if(_dep.log == "0")
	{
		return false;
	}
	return true;
}


void ChatClient::getListUser()
{
    _contacts.clear();
    _dep.cmd = "GET_USERS";
    sendData();
    while (true)
    {
        if (!recvData())
            break;

        if (_dep.cmd == "END")
            break;

        if (_dep.to.empty())
            continue;

        if (std::find(_contacts.begin(), _contacts.end(), _dep.to) == _contacts.end())
            _contacts.push_back(_dep.to);
    }
}

std::string ChatClient::getCurrentUser() const{ return _currentUser; }

const std::vector<std::string> ChatClient::getContacts() const { return _contacts; }

const std::vector<Message<std::string> > ChatClient::getMessages() const { return _messages; }

bool ChatClient::sendMessage(const std::string& to, const std::string& text)
{
    _currentCMD = "SEND_MESSAGE";
    _dep.cmd = _currentCMD;
    _dep.from = _currentUser;
    _dep.to = to;
    _dep.textMessage = text;

    return sendData();
}
void ChatClient::recvMessageFrom()
{
	_currentCMD = "RECV_MESSAGE";
	_dep.to = _currentUser;
	_dep.cmd = _currentCMD;
	sendData();
	recvData();
	_messages.emplace_back(_dep.from, _dep.to, _dep.textMessage);
}

void ChatClient::recvAllMessageFrom()
{
	_currentCMD = "RAM";
	_dep.cmd = _currentCMD;
	sendData();
	
	while(_dep.cmd != "END")
	{
		recvData();
		_messages.emplace_back(_dep.from, _dep.to, _dep.textMessage);
	}
}

void ChatClient::resetState()
{
	_currentCMD.clear();
	_currentUser.clear();
	_contacts.clear();
	_bGetListUsers = true;
	_dep.DataClear();
	_messages.clear();
	_dep.cmd = "EXIT";
}

bool ChatClient::requestAllMessages()
{
    _messages.clear();
    _dep.DataClear();
    _dep.cmd   = "GET_ALL_MESSAGES";
    _dep.login = _currentUser;

    if (!sendData())
        return false;

    while (true)
    {
        if (!recvData())
        {
            return false;
        }

        if (_dep.cmd == "CMD_STOP")
            break;

        if (_dep.from.empty() && _dep.to.empty() && _dep.textMessage.empty())
            continue;

        _messages.emplace_back(_dep.from, _dep.to, _dep.textMessage);
    }

    return true;
}
