#pragma once
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using SocketType = SOCKET;
#else
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //htonl ntohl
#include <unistd.h>
using SocketType = int;
#endif

#include "Message.h"
#include "DataExchangeProtocol.h"

#define MESSAGE_LENGTH 1024
#define PORT 63462


class ChatClient
{
public:

    ChatClient();
    ~ChatClient();

    bool SetupConnection();


    bool sendData();
    bool recvData();

    bool authUser(const std::string& login, const std::string& password);
    bool userRegister(const std::string& login, const std::string& password);
    void getListUser();
    std::string getCurrentUser() const;
    const std::vector<std::string>getContacts() const;
    const std::vector<Message<std::string>>getMessages() const;


    bool sendMessage(const std::string& to, const std::string& text);
    void recvMessageFrom();
    void recvAllMessageFrom();
    void resetState();
    void mainLoop();
    bool requestAllMessages();

private:

    DataExchangeProtocol _dep;
    
    std::vector<Message<std::string>>_messages;
    std::vector<std::string>_contacts;
    
    std::string _currentCMD;
    bool _bGetListUsers = true;

    std::string _currentUser;

    int _choise;    
    SocketType _socket_file_descriptor;
	int  _connection, _bytes_read;
	struct sockaddr_in _serveraddress, _client;
    
};

