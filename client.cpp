// Name:Tiancheng Luo
// USCID:8965950042
// -----------------------------------
// client.cpp:
// The client code that get all the input and connect to serverM for requests.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <vector>
#include "supportFunc.h"
#include <algorithm>

using namespace std;

#define HOSTNAME "studentVM"
#define MAXN 10000
#define SERVERS "41042"
#define SERVERL "42042"
#define SERVERH "43042"
#define SERVERMUDP "44042"
#define SERVERMTCP "45042"

struct Authorization//structure for login information.
{
	string username;
	string password;
};

Authorization loginInput(int sockfd, int loc_port)//
{
	Authorization user;

	while (true)
	{
		cout << "Please enter the username: ";
		cin >> user.username;
		cout << "Please enter the password: ";
		cin >> user.password;

		char res[MAXN];
		string data = user.username + "," + user.password;
		send(sockfd, data.c_str(), data.length(), 0);
		cout << user.username << " sent an authentication request to the Main Server.\n";
		recv(sockfd, res, MAXN - 1, 0);

		if (res[0] == '1')//Login valid.
		{
			cout << user.username << " received the result of authentication from Main Server using TCP over port " << loc_port << ".\nAuthentication is successful\n";
			break;
		}
		else if (res[0] == '2')//Username is wrong.
		{
			cout << user.username << " received the result of authentication from Main Server using TCP over port " << loc_port << ".\nAuthentication failed: Username not found.\n";
		}
		else if (res[0] == '3')//Password is wrong
		{
			cout << user.username << " received the result of authentication from Main Server using TCP over port " << loc_port << ".\nAuthentication failed: Password does not match.\n";
		}
		else
		{
			cout << "Server Down! Please retry!\n";
		}
	}

	send(sockfd, ",", 1, 0);//To break login while loop
	return user;
}

void working(Authorization usr, int sockfd, int loc_port)//Main working loop to process request.
{

	string inputCode, c;
	char x;

	while (true)//Query loop: always return to start to wait for input.
	{
		cout << "Please enter book code to query:";

		string inputCode = "";
		string tmpCode = "";
		cin >> inputCode;
		while ((x = cin.get()) != '\n')
		{
			inputCode = inputCode + x;
			if (vaildInput(x))
				inputCode.pop_back();
		}
		if (inputCode == "EXIT")
			break;

		tmpCode = inputCode;
		std::replace(inputCode.begin(), inputCode.end(), ',', 'X');
		
		char res[MAXN];
		int msg_len;

		if (usr.username == "Admin")//if Admin request, return for number.
		{
			string data = inputCode;
			send(sockfd, data.c_str(), data.length(), 0);
			cout << "Request sent to the Main Server with Admin rights.\n";
			msg_len = recv(sockfd, res, MAXN - 1, 0);
			cout << "Response received from the Main Server on TCP port:" << loc_port << ".\n";
			
			if (res[0] == '-' && res[1] == '1')//Wrong bookcode
			{
				cout << "Not able to find the book-code " << tmpCode.c_str() << " in the system.\n";
			}
			else//return the number
			{
				cout << "Total number of book " << tmpCode.c_str() << " available=" << res[0] <<"\n";
			}	
		}
		else// Ordinary request and ordinary return message.
		{
			string data = inputCode;
			send(sockfd, data.c_str(), data.length(), 0);
			cout << usr.username << " sent the request to the Main Server.\n";
			msg_len = recv(sockfd, res, MAXN - 1, 0);
			cout << "Response received from the Main Server on TCP port:" << loc_port << ".\n";

			if (res[0] == '-' && res[1] == '1')//Wrong bookcode
			{
				cout << "Not able to find the book-code " << tmpCode.c_str() << " in the system.\n";
			}
			else if (res[0] == '0')//number = 0
			{
				cout << "The requested book " << tmpCode.c_str() << " is NOT available in the library.\n";
			}
			else//number > 0
			{
				cout << "The requested book " << tmpCode.c_str() << " is available in the library.\n";
			}			
		}

		cout << "---Start a new request---\n";
	}
}

int main(void)
{
	int mainsocketfd = buildSock();//start socket
	struct sockaddr_in loc_add;
	socklen_t loc_add_len = sizeof(loc_add);
	memset(&loc_add, 0, loc_add_len);
	getsockname(mainsocketfd, (struct sockaddr *)&loc_add, &loc_add_len);
	int loc_port = ntohs(loc_add.sin_port); //get client port number.

	cout << "Client is up and running.\n";//start request
	Authorization usr = loginInput(mainsocketfd, loc_port);
	working(usr, mainsocketfd, loc_port);//start answer
	cout << "\nClient will shut down.\n";
	close(mainsocketfd);

	return 0;
}
