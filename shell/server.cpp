#include <iostream>
#include <sys/socket.h> //library for server-client communication
using namespace std;

int main()
{
    struct sockaddr *servaddr; // IPv4 socket address structure
    socklen_t addrlen = sizeof(servaddr);
    char *buff = "Hello world"; // message to send to client

    // for server we need socket(), bind(), listen(), accept(), read(), write() function calls to connect with client

    int socketfd = socket(AF_INET, SOCK_STREAM, 0); // create socket. AF_INET for ipv4, SOCK_STREAM for TCP connection, 0 for protocol
    if (socketfd == -1)
    {
        cout << "socket creation failed";
        exit(1);
    }

    int bindfd = connect(socketfd, servaddr, addrlen); // bind to specific IP addr. servaddr is the specified IP addr, and addrlen is the length of the IP addr
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

    // int acceptfd = accept(socketfd, servaddr, (socklen_t *)addrlen); // new socket after connect() is called on client side
    int acceptfd = accept(socketfd, NULL, NULL); // set clientaddr and sizeof(clientaddr) to NULL, disregard client's identity
    if (acceptfd == -1)
    {
        cout << "accept failed";
        exit(1);
    }

    int readfd = recv(acceptfd, buff, addrlen, 0);
    if (readfd == -1)
    {
        cout << "read failed";
        exit(1);
    }

    send(acceptfd, buff, addrlen, 0); // send message to client
    if (readfd == -1)
    {
        cout << "write failed";
        exit(1);
    }
}