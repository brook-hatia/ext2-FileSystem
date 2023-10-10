#include <iostream>
#include <sys/socket.h> //library for server-client communication
#include <netinet/in.h> //for serveaddr_in which is used for IPv4
#include <unistd.h>     //for close()
using namespace std;

int main()
{
    struct sockaddr_in servaddr;     // the "_in" in sockaddr_in is IPv4 socket address structure
    servaddr.sin_family = AF_INET;   // IPv4
    servaddr.sin_port = htons(8080); // port number
    servaddr.sin_addr.s_addr = INADDR_ANY;

    socklen_t addrlen = sizeof(servaddr);
    char rcvMsg[100] = {0};              // allocated space for receiving from client
    const char *sendMsg = "Hello world"; // message to send to client

    // for server we need socket(), bind(), listen(), accept(), read(), write() function calls to connect with client

    int socketfd = socket(AF_INET, SOCK_STREAM, 0); // create socket. AF_INET for ipv4, SOCK_STREAM for TCP connection, 0 for protocol
    if (socketfd == -1)
    {
        cout << "socket creation failed";
        exit(1);
    }
    // bind to specific IP addr. servaddr is the specified IP addr, and addrlen is the length of the IP addr
    int bindfd = bind(socketfd, (sockaddr *)&servaddr, addrlen);
    if (bindfd == -1)
    {
        cout << "bind failed";
        exit(1);
    }

    int listenfd = listen(socketfd, 5); // listen to incoming connection. 5 is the backlog of incoming message in queue
    if (listenfd == -1)
    {
        cout << "listen failed";
        exit(1);
    }

    int acceptfd = accept(socketfd, (struct sockaddr *)&servaddr, (socklen_t *)&addrlen); // new socket after connect() is called on client side
    if (acceptfd == -1)
    {
        cout << "accept failed";
        exit(1);
    }

    int readfd = recv(acceptfd, rcvMsg, sizeof(rcvMsg), 0); // receive message from client
    if (readfd == -1)
    {
        cout << "read failed";
        exit(1);
    }

    int writefd = send(acceptfd, sendMsg, strlen(sendMsg), 0); // send message to client
    if (readfd == -1)
    {
        cout << "write failed";
        exit(1);
    }

    close(socketfd); // close socket
}