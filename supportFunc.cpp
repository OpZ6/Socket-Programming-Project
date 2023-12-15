// Name:Tiancheng Luo
// USCID:8965950042
// -----------------------------------
// supportFunc.cpp:
// All the functions that needed for all the clients' code and servers' code.

#include "supportFunc.h"
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

#define HOSTNAME "studentVM"
#define MAXN 10000
#define SERVERS "41042"
#define SERVERL "42042"
#define SERVERH "43042"
#define SERVERMUDP "44042"
#define SERVERMTCP "45042"

using namespace std;

void UDP_SEND(string message, string port)//UDP send function
{
	int udpSocket;
	struct addrinfo hints;
	struct addrinfo *serverInfo;
	struct addrinfo *pServerInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;
	getaddrinfo("localhost", port.c_str(), &hints, &serverInfo);
	for (pServerInfo = serverInfo; pServerInfo != NULL; pServerInfo = pServerInfo->ai_next)
	{
		udpSocket = socket(pServerInfo->ai_family, pServerInfo->ai_socktype, pServerInfo->ai_protocol);
		if (udpSocket != -1)
		{
			break;
		}
	}
	if (pServerInfo != NULL)
	{
		sendto(udpSocket, message.c_str(), message.length(), 0, pServerInfo->ai_addr, pServerInfo->ai_addrlen);
		freeaddrinfo(serverInfo);
		close(udpSocket);
	}
	else
	{
		cout << "Create UDP socket failed.\n";
	}
}

string UDP_REC(string port)//UDP receive function
{
	int sockUDP, bindUDP, message_lenth;
	struct addrinfo pre;
	struct addrinfo *ser;
	struct addrinfo *pser;
	struct sockaddr_storage STaddr;
	char info[MAXN];
	string result;
	socklen_t addlen;

	memset(&pre, 0, sizeof(pre));
	pre.ai_family = AF_INET;
	pre.ai_socktype = SOCK_DGRAM;
	pre.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, port.c_str(), &pre, &ser);

	for (pser = ser; pser != NULL; pser = pser->ai_next)
	{
		sockUDP = socket(pser->ai_family, pser->ai_socktype, pser->ai_protocol);
		if (sockUDP != -1)
		{
			bindUDP = bind(sockUDP, pser->ai_addr, pser->ai_addrlen);
			if (bindUDP != -1)
				break;
		}
	}

	freeaddrinfo(ser);
	addlen = sizeof(STaddr);
	message_lenth = recvfrom(sockUDP, info, MAXN - 1, 0, (struct sockaddr *)&STaddr, &addlen);
	info[message_lenth] = '\0';
	for (int i = 0; i < message_lenth; i++)
	{
		result = result + info[i];		
	}
	close(sockUDP);
	return result;
}

int buildSock()//Socket build function
{
	int mainsocketfd, connection;
	struct addrinfo setting;
	struct addrinfo *server;
	struct addrinfo *iter;

	memset(&setting, 0, sizeof setting);
	setting.ai_family = AF_INET;
	setting.ai_socktype = SOCK_STREAM;
	getaddrinfo(HOSTNAME, SERVERMTCP, &setting, &server);

	for (iter = server; iter != NULL; iter = iter->ai_next)
	{
		mainsocketfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
		if (mainsocketfd != -1)
		{
			connection = connect(mainsocketfd, iter->ai_addr, iter->ai_addrlen);
			if (connection != -1)
			{
				break;				
			}
		}
	}

	freeaddrinfo(server);
	return mainsocketfd;
}

bool vaildInput(char c)//input check function
{
	if (c == '\n'||c == '\t'||c == '\f'||c == '\b'
		||c == '\r'||c == '\v'||c == '\a'||c == '\0'||c ==',')
	{
		return true;			
	}
	return false;
}

string encrypt(string str)//encrypt password and username.
{
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			str[i] = 65 + (str[i] - 60) % 26;
		}
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] = 97 + (str[i] - 92) % 26;			
		}
		if (str[i] >= '0' && str[i] <= '9')
		{
			str[i] = 48+ (str[i] - 43) % 10;
		}
	}
	return str;
}

map<string, string> readUsr(string path)//username and password data read function
{
	string line, usn, pwd;
	map<string, string> usr;
	const char *filename = path.c_str();
	freopen(filename, "r", stdin);
	while ((getline(cin, line)))
	{
		if (vaildInput(line[line.length() - 1]))
			line.pop_back();
		usn = line.substr(0, line.find(','));
		pwd = line.substr(line.find(',') + 2);
		usr[usn] = pwd;
	}
	fclose(stdin);
	return usr;
}

map<string, string> readData(string path)//book data read function
{
	string line, bookCode, bookNum;
	map<string, string> bookList;
	const char *filename = path.c_str();
	freopen (filename, "r", stdin);
	while ((getline(cin, line)))
	{
		if (vaildInput(line[line.length() - 1]))
			line.pop_back();
		bookCode = line.substr(0, line.find(','));
		bookNum = line.substr(line.find(',') + 2);
		bookList[bookCode] = bookNum;
	}
    fclose(stdin);
	return bookList;
}