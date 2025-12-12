#include "ChatServer.h"


//CHAT-Socket
Chat::Chat() : socket_file_descriptor(-1), bind_status(-1), connection_status(-1)
{
    _db = std::make_unique<Database>();
    if (!_db->isOpen()) {
        std::cout << "Open DB failed!\n";
    }
}
Chat::~Chat()
{
#ifdef _WIN32
	if (socket_file_descriptor != INVALID_SOCKET)
		closesocket(socket_file_descriptor);
#else
	if (socket_file_descriptor != -1)
		close(socket_file_descriptor);
#endif
}
bool Chat::run()
{
    _running = true;

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

#ifdef _WIN32
    if (socket_file_descriptor == INVALID_SOCKET)
#else
    if (socket_file_descriptor == -1)
#endif
    {
        perror("Socket creation failed!");
        return false;
    }

    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddress.sin_port = htons(PORT);
    serveraddress.sin_family = AF_INET;

    bind_status = bind(socket_file_descriptor,
                       (struct sockaddr*)&serveraddress,
                       sizeof(serveraddress));
#ifdef _WIN32
    if (bind_status == SOCKET_ERROR)
#else
    if (bind_status == -1)
#endif
    {
        perror("Socket binding status!");
        closeServerSocket();
        return false;
    }

    connection_status = listen(socket_file_descriptor, 5);

#ifdef _WIN32
    if (connection_status == SOCKET_ERROR)
#else
    if (connection_status == -1)
#endif
    {
        std::cout << "Socket is unable to listen for new connections.!" << std::endl;
        closeServerSocket();
        return false;
    }

    std::cout << "Server is listening for new connection." << std::endl;
    return true;
}
void Chat::closeServerSocket()
{

#ifdef _WIN32
        if (socket_file_descriptor != INVALID_SOCKET)
            closesocket(socket_file_descriptor);
        socket_file_descriptor = INVALID_SOCKET;
#else
        if (socket_file_descriptor != -1)
            close(socket_file_descriptor);
        socket_file_descriptor = -1;
#endif

}
void Chat::stop()
{
    _running = false;
    closeServerSocket();
}
SocketType Chat::acceptClient()
{
    struct sockaddr_in client;
    socklen_t length = sizeof(client);

    SocketType connection = accept(socket_file_descriptor,
                                   (struct sockaddr*)&client,
                                   &length);

#ifdef _WIN32
    if (connection == INVALID_SOCKET)
#else
    if (connection < 0)
#endif
    {
        perror("Server is unable to accept the data from client.!");
        return connection;
    }

    std::cout << "Client accepted\n";
    return connection;
}
bool Chat::recvData(int client_fd, DataExchangeProtocol& dep)
{
	char buf[MESSAGE_LENGTH];
	int n = recv(client_fd, buf, sizeof(buf),0);
	if (n <= 0) 
	{
		std::cout << "Disconnect\n";
		return false;
	}
	dep.DataClear();
	dep.inputDataHandler(buf, n);
    dep.showData();
	return true;
}
bool Chat::sendData(int client_fd, DataExchangeProtocol& dep)
{
    char buf[MESSAGE_LENGTH];
    dep.outputDataHandler(buf);
    int n = send(client_fd, buf, sizeof(buf), 0);
    if(n < 0)
        return false;

    return true;
}

//Chat
//User
void Chat::addUser(DataExchangeProtocol& dep)
{
    if (_db)
    {
        if (!_db->addUser(dep.login, dep.password))
        {
            std::cout << "Failed to add user to DB\n";
            dep.log = "0";
            return;
        }
    }

    std::cout << "User added successfully" << std::endl;
    dep.log = "1";
}

bool Chat::loginUser(int client_fd, DataExchangeProtocol &dep)
{
    if (!_db || !_db->isOpen())
    {
        dep.log = "0";
        return false;
    }

    if (_db->isBanned(dep.login))
    {
        dep.log = "BANNED";
        return false;
    }

    if (!_db->checkPassword(dep.login, dep.password))
    {
        dep.log = "0";
        return false;
    }
    _db->setOnline(dep.login, true);

    dep.log = "1";
    return true;
}
bool Chat::validateAddUser(std::string& login) const
{
    std::cout << login << std::endl;

    if (_db && _db->userExist(login))
    {
        std::cout << "Имя занято (в БД)\n";
        return false;
    }
    return true;
}
std::vector<std::string> Chat::getAllUsers() const
{
    std::vector<std::string> users;
    if (_db)
        _db->getAllUsers(users);
    return users;
}

bool Chat::sendAllMessages(int client_fd, DataExchangeProtocol& dep)
{
    //dep.cmd = "ALL_MESSAGES";

    std::vector<Message<std::string>> msgs;

    if (_db)
        _db->getMessagesForUser(dep.login, msgs);

    for (const auto& msg : msgs)
    {
        dep.from = msg.getFrom();
        dep.to = msg.getTo();
        dep.textMessage = msg.getMessage();

        sendData(client_fd, dep);
        dep.DataClear();
    }

    dep.cmd = "CMD_STOP";
    sendData(client_fd, dep);

    return true;
}
void Chat::sendMessageToClient(int client_fd, DataExchangeProtocol& dep)
{
    if (dep.textMessage == "exit")
        return;

    std::vector<Message<std::string>> msgs;

    if (_db)
        _db->getMessagesForUser(dep.login, msgs);

    for (const auto& m : msgs)
    {
        dep.textMessage = m.getMessage();
        dep.from = m.getFrom();
        dep.to = m.getTo();
        sendData(client_fd, dep);
    }

    dep.DataClear();
    dep.cmd = "END";
    sendData(client_fd, dep);
}

void Chat::sendUsersToClient(int client_fd, DataExchangeProtocol &dep)
{

    auto users = getUsers();

    for (const auto& user : users)
    {
        //dep.cmd   = "USER";
        dep.to = user.getName();
        sendData(client_fd, dep);
        dep.DataClear();
    }

    dep.cmd = "END";
    sendData(client_fd, dep);
}

void Chat::markUserOffline(const std::string &login)
{
    if (_db) _db->setOnline(login, false);
}
std::vector<Message<std::string> > Chat::getAllMessages() const
{
    std::vector<Message<std::string>> msgs;
    if (_db)
        _db->getAllMessages(msgs);
    return msgs;
}

bool Chat::banCheck(const std::string &login) const
{
    return _db ? _db->isBanned(login) : false;
}

void Chat::banUser(const std::string &login)
{
    if (_db) _db->setBanned(login, true);
}

void Chat::unbanUser(const std::string &login)
{
    if (_db) _db->setBanned(login, false);
}

void Chat::setUserOnline(const std::string &login, bool online)
{
    if (_db) _db->setOnline(login, online);
}

void Chat::recvMessageToServer(const DataExchangeProtocol &dep)
{
    if (!_db) return;
    _db->addMessage(dep.from, dep.textMessage, dep.to);
}

std::vector<User> Chat::getUsers() const
{
    std::vector<User> result;
    if (!_db) return result;

    std::vector<std::string> logins;
    if (!_db->getAllUsers(logins))
        return result;

    result.reserve(logins.size());

    for (const auto& login : logins)
    {
        User u(login);

        // banned
        u.setBanned(_db->isBanned(login));

        // online (если у тебя есть isOnline в Database)
        u.setOnline(_db->isOnline(login));

        result.push_back(u);
    }
    return result;

}

std::vector<Message<std::string> > Chat::getMessagesForUser(const std::string &login) const
{
    std::vector<Message<std::string>> msgs;

    if (_db)
        _db->getMessagesForUser(login, msgs);

    return msgs;
}
bool Chat::isRunning() const
{
    return _running;
}


	




