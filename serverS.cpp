// Name:Tiancheng Luo
// USCID:8965950042
// -----------------------------------
// serverS.cpp:
// The backend server for Science Book data and work on search request from the main server.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "supportFunc.h"

#define HOSTNAME "studentVM"
#define MAXN 10000
#define SERVERS "41042"
#define SERVERL "42042"
#define SERVERH "43042"
#define SERVERMUDP "44042"
#define SERVERMTCP "45042"

#define MAXN 10000

using namespace std;

int main(void)
{
	cout << "ServerS is up and running using UDP on port " << SERVERS << ".\n";
	map<string, string> bookList = readData("science.txt");

	string message = "serverS:";
	for (const auto& entry : bookList) {
		message = message + entry.first +",";
	}
	//cout<<message<<"\n";

	UDP_SEND(message, SERVERMUDP);//transfer bookcode list to the main server.

	while (true)//start listening for search request.
	{
		string info = UDP_REC(SERVERS);
		//cout << "The info: " << info << ".\n";
		string bookCode = info.substr(0, info.find(','));
		cout << "ServerS received " << bookCode << " code from the Main Server.\n";

		if (bookList.find(bookCode) == bookList.end())//not found data
		{
			//cout << "Did not find " << bookCode << " in the book list.\n";
			UDP_SEND("-1", SERVERMUDP);
		}
		else//found data
		{
			//cout << "The course information has been found: " << bookCode << " with " << bookList[bookCode] << " available.\n";
			UDP_SEND(bookList[bookCode], SERVERMUDP);
		}

		cout << "ServerS finished sending the availability status of code " << bookCode << " to the Main Server using UDP on port " << SERVERMUDP << ".\n";
	}
	
	return 0;
}
