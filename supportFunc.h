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
#include <vector>
#pragma once

using namespace std;

void UDP_SEND(string message, string port);
string UDP_REC(string port);
int buildSock();
bool vaildInput (char c);
string encrypt(string str);
map<string, string> readUsr(string path);
map<string, string> readData(string path);