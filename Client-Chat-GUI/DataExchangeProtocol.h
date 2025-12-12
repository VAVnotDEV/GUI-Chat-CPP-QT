#pragma once
#include <iostream>


struct DataExchangeProtocol
{	
    //CMD:LOG:LOGIN:PASSWORD:TO:FROM:TEXTMESSAGE
    std::string cmd;
    std::string log;
    std::string login;
    std::string password;
    std::string to;
    std::string from;
    std::string textMessage;	

    void inputDataHandler(char* ch1, int n);
    void outputDataHandler(char* ch1) const;
    void DataClear();
    void showData() const;
};