# Socket-Programming-Project
Socket Programming Project for Computer Network Class
Use For TCLuo EE450 2023FALL

c. What I did:
I build all the client and 4 server code, also satisfy the optional part by login with Admin account. The program can act well under all situation, but please make sure to start them in the right order mentioned in the PDF:ServerM, ServerS, ServerL, ServerH, and client.


d. File Description:
// client.cpp:
// The client code that get all the input and connect to serverM for requests.

// supportFunc.cpp:
// All the functions that needed for all the clients' code and servers' code.

// supportFunc.h:
// header file of supportFunc.cpp.

// serverM.cpp:
// The Main Server that get all the request from client and other backend servers for search request.

// serverS.cpp:
// The backend server for Science Book data and work on search request from the main server.

// serverL.cpp:
// The backend server for Literature Book data and work on search request from the main server.

// serverH.cpp:
// The backend server for History Book data and work on search request from the main server.


e. The format of all the messages exchanged:
- severM TO client: 
    number of login status:
        "1": Login valid,
        "2": Username is wrong,
        "3": Password is wrong.
    the Book's Availble number:
        "-1": Wrong bookcode,
        "0": Unavailable,
        all number >1: Actual available number.

- client to serverM:
    message of username and password:
        Concatenated together and divided by ",".
    message of BookCode:
        Directly but with input valid check.

- serverM to backend server<S,L,H>:
    message of search BookCode:
        Directly.

- backend server<S,L,H> to serverM:
    message of BookCode List:
        ServerName + ":" + BookCode divided by ",".
    reply of Available:
        Directly of the BookCode's actual available number.


g. Idiosyncrasy:
Under no conditions the project would fail.


h. Reference Code:
- The majority of code for TCP and UDP sockets referred to Beej's Guide to Network Programming (socket programming). Including socket(),connnet(),bind(),accept()etc.
- They are used in serverM.cpp: main() AND supportFunc.cpp: buildSock(), UDP_REC(), UDP_SEND().


l. Running Steps:
1. Enter into command line of the root directory.
2. Run "make all".
3. Open four terminals to run "./serverM", "./serverS", "./serverL" and "./serverH"(serverM must be first)
4. Open a terminal to run "./client".
5. In client terminal enter username and password.
6. Then continue enter the BookCode.
7. When finish, just use "ctrl+C" command OR you can type "EXIT".
