#include <iostream>
#include <sys/socket.h> //library for server-client communication
#include <netinet/in.h> //for serveaddr_in which is used for IPv4
#include <unistd.h>     //for close()
#include "fs.h"
using namespace std;

// string ls()
// {
//     return "ls success";
// }

// string cd(std::string str)
// {
//     return "cd success, parameter: " + str;
// }

// string mkdir(std::string str)
// {
//     return "mkdir success, parameter: " + str;
// }

// string lcp(std::string str)
// {
//     return "lcp success, parameter: " + str;
// }

// string Lcp(std::string str)
// {
//     return "Lcp success, parameter: " + str;
// }

// string shutdown()
// {
//     return "shutdown success";
// }

// string exit()
// {
//     return "exit success";
// }

string *scan(char *parameter)
{
    string str_param(parameter); // convert char array to string
    string *identify = new string[3];
    int j = 0;

    for (int i = 0; i < str_param.size(); i++)
    {
        if (str_param[i] == ' ')
        {
            j++;
        }

        identify[j] += str_param[i];
    }

    return identify;
}

string identify_function(string *prompt)
{
    string rc;
    if (prompt[0] == "ls")
    {
        rc = ls();
    }

    else if (prompt[0] == "cd")
    {
        rc = cd(prompt[1]);
    }

    else if (prompt[0] == "mkdir")
    {
        rc = my_mkdir(prompt[1]);
    }

    else if (prompt[0] == "lcp")
    {
        rc = lcp(prompt[1]);
    }

    else if (prompt[0] == "Lcp")
    {
        rc = Lcp(prompt[1]);
    }

    else if (prompt[0] == "shutdown")
    {
        rc = shutdown();
    }

    else if (prompt[0] == "exit")
    {
        rc = exit();
    }

    else
    {
        rc = "command not found";
    }

    return rc;
}

int main()
{
    struct sockaddr_in servaddr; // the "_in" in sockaddr_in is IPv4 socket address structure

    // initialize servaddr
    servaddr.sin_family = AF_INET;         // IPv4
    servaddr.sin_port = 8080;              // port number. "host to network short"
    servaddr.sin_addr.s_addr = INADDR_ANY; // bind to any available address

    // for server we need socket(), bind(), listen(), accept(), read(), write() function calls

    int socketfd = socket(AF_INET, SOCK_STREAM, 0); // create socket. AF_INET for ipv4, SOCK_STREAM for TCP connection, 0 for protocol

    int bindfd = bind(socketfd, (sockaddr *)&servaddr, (socklen_t)sizeof(servaddr)); // bind to specific IP addr. servaddr is the specified IP addr, and the socket length of the servaddr in bytes

    int listenfd = listen(socketfd, 1); // listen to incoming connection. 1 is the backlog of incoming message in queue
    cout << "listening to connection ..." << endl;

    socklen_t servaddrLen = sizeof(servaddr);                                          // size of servaddr in bytes
    int acceptfd = accept(socketfd, (sockaddr *)&servaddr, (socklen_t *)&servaddrLen); // new socket after connect() is called on client side

    if (socketfd == -1 || bindfd == -1 || listenfd == -1 || acceptfd == -1)
    { // error returns -1
        cout << "connection failed" << endl;
        return 1;
    }
    cout << "connection success" << endl;

    while (true)
    {
        char readMsg[4000];                                            // allocated space for receiving from client
        int readfd = read(acceptfd, readMsg, (size_t)sizeof(readMsg)); // receive message from client
        cout << "\nClient: " << readMsg << endl;

        cout << "Server shell: ";

        // char sendMsg[1024]; // allocate space for send message

        // scan the read message for function name, filename/path
        string *contents = scan(readMsg);

        string sendMsg = identify_function(contents);
        // getline(cin, sendMsg); // server prompt

        if (sendMsg == "exit")
        {
            break;
        }

        int writefd = write(acceptfd, sendMsg.c_str(), (size_t)sendMsg.size()); // send message to client
        // cout << "\nwrite successful" << sendMsg;
    }

    close(socketfd); // close client's socket
}