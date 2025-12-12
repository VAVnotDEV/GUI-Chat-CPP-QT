#include "DataExchangeProtocol.h"
#include <string.h>

#define MESSAGE_LENGTH 1024

void DataExchangeProtocol::inputDataHandler(char* ch1, int n)
{
    DataClear();
	//CMD:LOG:LOGIN:PASSWORD:TO:FROM:TEXTMESSAGE
	int count = 0;
	char sp = ':';

    for (int i = 0; i < n && ch1[i] != '\0'; ++i)
    {
        if (ch1[i] == sp && count < 6)
        {
            count++;
            continue;
        }

        if (count == 0)
            cmd.push_back(ch1[i]);
        else if (count == 1)
            log.push_back(ch1[i]);
        else if (count == 2)
            login.push_back(ch1[i]);
        else if (count == 3)
            password.push_back(ch1[i]);
        else if (count == 4)
            to.push_back(ch1[i]);
        else if (count == 5)
            from.push_back(ch1[i]);
        else if (count == 6)
            textMessage.push_back(ch1[i]);
    }
	
}

void DataExchangeProtocol::outputDataHandler(char* ch1) const
{
	//CMD:LOG:LOGIN:PASSWORD:TO:FROM:TEXTMESSAGE
	memset(ch1, 0, MESSAGE_LENGTH);
	char sp = ':';
	std::string fullData = cmd + sp + log + sp + login + sp + password + sp + to + sp + from + sp + textMessage;

#ifdef _WIN32

	strcpy_s(ch1, MESSAGE_LENGTH, fullData.c_str());
#else
	strcpy(ch1, fullData.c_str());
#endif
	//ch1[fullData.size()] = '\0';

}

void DataExchangeProtocol::showData() const
{
			std::cout << "*****DEP BEGIN*****\n";
			if(!cmd.empty())
				std::cout << "CMD: " << cmd << std::endl;
			
				if(!log.empty())
				std::cout << "Log: " << log << std::endl;

			if(!login.empty())
				std::cout << "Login: " << login << std::endl;
			
			if(!password.empty())
				std::cout << "Password: " << password << std::endl;
			
			if(!to.empty())
				std::cout << "To: " << to << std::endl;
			
			if(!from.empty())
				std::cout << "From: " << from << std::endl;
			
			if(!textMessage.empty())
				std::cout << "TextMessage: " << textMessage << std::endl;
				std::cout << "*****DEP_END*****\n";
}

void DataExchangeProtocol::DataClear()
{
	cmd.clear();
	log.clear();
	login.clear();
	password.clear();
	to.clear();
	from.clear();
	textMessage.clear();
}
