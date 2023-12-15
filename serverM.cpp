// Name:Tiancheng Luo
// USCID:8965950042
// -----------------------------------
// serverM.cpp:
// The Main Server that get all the request from client and other backend servers for search request.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <map>
#include "supportFunc.h"

#define HOSTNAME "studentVM"
#define MAXN 10000
#define SERVERS "41042"
#define SERVERL "42042"
#define SERVERH "43042"
#define SERVERMUDP "44042"
#define SERVERMTCP "45042"

using namespace std;

int main(void)
{
	int mainsocketfd, subsockfd, bindsig, opt, T = 1, msg_len;
	struct addrinfo setting;
	struct addrinfo *server;
	struct addrinfo *iter;
	struct sockaddr_storage STaddr;
	socklen_t addlen;

	memset(&setting, 0, sizeof(setting));//socket
	setting.ai_family = AF_INET;
	setting.ai_socktype = SOCK_STREAM;
	setting.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, SERVERMTCP, &setting, &server);
	for (iter = server; iter != NULL; iter = iter->ai_next)
	{
		mainsocketfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
		if (mainsocketfd != -1)
		{
			opt = setsockopt(mainsocketfd, SOL_SOCKET, SO_REUSEADDR, &T, sizeof(int));
			if (opt != -1)
			{
				bind(mainsocketfd, iter->ai_addr, iter->ai_addrlen);
				if (bindsig != -1)
					break;
				else
					close(mainsocketfd);
			}
		}
	}
	freeaddrinfo(server);//finish

	if (iter == NULL)//test
	{
		fprintf(stderr, "Bind Error!\n");
		exit(1);
	}

	listen(mainsocketfd, 15);//start server

	cout << "Main server is up and running.\n";

	string SList="";
	string LList="";
	string HList="";

	int i = 0;
	while (true)//get backend server information.
	{
		string result;
		result = UDP_REC(SERVERMUDP);

		string serverName = result.substr(0, result.find(':'));
		string tmpBookList = result.substr(result.find(':') + 1);
		// cout<<"DATA---"<<serverName<<"\n";
		// cout<<"DATA---"<<tmpBookList<<"\n";

		if(serverName == "serverS")
		{
			SList = tmpBookList;
			cout << "Main Server received the book code list from " << serverName <<" using UDP over port " << SERVERMUDP <<".\n";
		}
		else if(serverName == "serverL")
		{
			LList = tmpBookList;
			cout << "Main Server received the book code list from " << serverName <<" using UDP over port " << SERVERMUDP <<".\n";
		}
		else if(serverName == "serverH")
		{
			HList = tmpBookList;
			cout << "Main Server received the book code list from " << serverName <<" using UDP over port " << SERVERMUDP <<".\n";
		}
		else
		{
			cout<<"Server Name Error!";
			break;
		}
		i += 1;
		//cout<< i << "\n";
		if (i == 3)
		{
			//cout << "Break!\n";
			break;			
		}
	}

	while (true)//start login and search loop.
	{	
		addlen = sizeof STaddr;
		subsockfd = accept(mainsocketfd, (struct sockaddr *)&STaddr, &addlen);

		if (subsockfd == -1)
		{
			perror("Accept!");
			continue;
		}

		if (!fork())//multithreading reduce delay
		{
			close(mainsocketfd);
			char data[MAXN], query[MAXN], res[MAXN];
			res[0] = '1';
			string usn, pwd, bookCode, bookNum;
			map<string, string> usr = readUsr("member.txt");
			cout << "Main server loaded the member list.\n";//finish reading the member data

			while ((msg_len = recv(subsockfd, data, MAXN - 1, 0)) != -1) //receive login information.
			{
				
				if (msg_len == 1 && data[0] == ',')
				{
					break;
				}

				cout << "Main Server received the username and password from the client using TCP over port " << SERVERMTCP << ".\n";
				data[msg_len] = '\0';
				usn = "";
				pwd = "";
				string tmpUsn = "";
				string tmpPwd = "";

				int pos = 0;
				while (pos < msg_len && data[pos] != ',')
				{
					usn = usn + data[pos];
					pos++;
				}
				pos++;
				while (pos < msg_len)
				{
					pwd = pwd + data[pos];
					pos++;
				}
				//cout << " start encrypt .\n";
				
				tmpUsn = usn;
				tmpPwd = pwd;
				usn = encrypt(usn);
				pwd = encrypt(pwd);//finish encrypt data
				
				if (usr.find(usn) == usr.end())//Username invalid.
				{
					cout << tmpUsn <<" is not registered. Send a reply to the client.\n";
					res[0] = '2';
				}
				else if (usr[usn] != pwd)//Password invalid.
				{
					cout << "Password "<< tmpPwd <<" does not match the username. Send a reply to the client.\n";
					res[0] = '3';
				}
				else//Login Successfully.
				{
					cout << "Password "<< tmpPwd <<" matches the username. Send a reply to the client.\n";
					res[0] = '1';
				}
				usn = tmpUsn;//finish login check

				send(subsockfd, res, 5, 0);
			}

			while ((msg_len = recv(subsockfd, query, MAXN - 1, 0)) > 0) //receive search book request.
			{
				query[msg_len] = '\0';
				bookCode = "";
				bookNum = "";
				int pos = 0;
				while (pos < msg_len && query[pos] != ',')
				{
					bookCode = bookCode + query[pos];
					pos++;
				}

				cout << "Main Server received the book request from client using TCP over port " << SERVERMTCP << ".\n";
				string result;
				if (bookCode.size() > 1 && SList.find(bookCode) != std::string::npos)//check Science bookcode and request
				{
					UDP_SEND(bookCode + ",", SERVERS);
					cout << "Found " << bookCode << " located at ServerS. Send to ServerS.\n";
					result = UDP_REC(SERVERMUDP);
					cout << "Main Server received from serverS the book status result using UDP over port " << SERVERMUDP << ":\n";
					cout << "Number of books " << bookCode << " available is: " << result << ".\n";
				}
				else if (bookCode.size() > 1 && LList.find(bookCode) != std::string::npos)//check Literature bookcode and request
				{
					UDP_SEND(bookCode + ",", SERVERL);
					cout << "Found " << bookCode << " located at ServerL. Send to ServerL.\n";
					result = UDP_REC(SERVERMUDP);
					cout << "Main Server received from serverL the book status result using UDP over port " << SERVERMUDP << ":\n";
					cout << "Number of books " << bookCode << " available is: " << result << ".\n";
				}
				else if (bookCode.size() > 1 && HList.find(bookCode) != std::string::npos)//check History bookcode and request
				{
					UDP_SEND(bookCode + ",", SERVERH);
					cout << "Found " << bookCode << " located at ServerH. Send to ServerH.\n";
					result = UDP_REC(SERVERMUDP);
					cout << "Main Server received from serverH the book status result using UDP over port " << SERVERMUDP << ":\n";
					cout << "Number of books " << bookCode << " available is: " << result << ".\n";
				}
				else//not valid book code
				{
					cout << "Did not find "<< bookCode << " in the book code list.\n";
					result = "-1";
				}
				send(subsockfd, result.c_str(), result.length(), 0);
				cout << "Main Server sent the book status to the client.\n";
			}
			close(subsockfd);
			exit(0);
		}
		close(subsockfd);
	}

	return 0;
}
