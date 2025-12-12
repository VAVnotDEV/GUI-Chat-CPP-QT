#pragma once
#include "Message.h"
#include "DataExchangeProtocol.h"
#include <vector>
#include <limits>
#include <iostream>
#include <string.h>
#include "database.h"

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using SocketType = SOCKET;

#else	
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using SocketType = int;
#endif

#define MESSAGE_LENGTH 1024
#define PORT 63462



class Chat
{

public:

	Chat();
	~Chat();
	//Inet Logic
    bool run();
    void closeServerSocket();
    void stop();
	SocketType acceptClient();
	bool recvData(int client_fd, DataExchangeProtocol& dep);
	bool sendData(int client_fd, DataExchangeProtocol& dep);
	
	//Chat Logic
    //User
	void addUser(DataExchangeProtocol& dep); 
	bool loginUser(int client_fd, DataExchangeProtocol& dep);
    bool validateAddUser(std::string& login) const;
    std::vector<std::string> getAllUsers() const;
    void sendUsersToClient(int client_fd, DataExchangeProtocol& dep);
    void markUserOffline(const std::string& login);
    //Message
	void sendMessageToClient(int client_fd, DataExchangeProtocol& dep);
    bool sendAllMessages(int client_fd, DataExchangeProtocol& dep);
	void displayAllMessages(const std::string& from, const std::string& to) const;
    std::vector<User> getAllUsersWithOnlineStatus() const;
    std::vector<Message<std::string>> getAllMessages() const;
    bool banCheck(const std::string& login) const;
    void banUser(const std::string& login);
    void unbanUser(const std::string& login);

    void setUserOnline(const std::string& login, bool online);

    void recvMessageToServer(const DataExchangeProtocol& dep);
    //Server GUI
    std::vector<User> getUsers() const;
    std::vector<Message<std::string>> getMessagesForUser(const std::string& login) const;

    bool isRunning() const;

private:
    bool _running = false;
	struct sockaddr_in serveraddress;
	SocketType socket_file_descriptor;
    int bind_status, connection_status;
    std::unique_ptr<Database> _db;
};

