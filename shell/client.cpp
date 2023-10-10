#include <iostream>
#include <sys/socket.h> //library for server-client communication
#include <netinet/in.h> //for serveaddr_in which is used for IPv4
#include <unistd.h>     //for close()
using namespace std;

int main()
{
    struct sockaddr_in servaddr; // the "_in" in sockaddr_in is IPv4 socket address structure

    // initialize servaddr
    servaddr.sin_family = AF_INET;         // IPv4
    servaddr.sin_port = htons(8080);       // port number. "host to network short"
    servaddr.sin_addr.s_addr = INADDR_ANY; // bind to server's IP addr
    socklen_t addrlen = sizeof(servaddr);
    char buff[100] = {0}; // allocate space to read from server

    // for client we need socket(), connect(), send(), receive()

    int socketfd = socket(AF_INET, SOCK_STREAM, 0); // create socket. AF_INET for ipv4, SOCK_STREAM for TCP connection, 0 for protocol
    if (socketfd == -1)
    {
        cout << "socket creation failed";
        exit(1);
    }

    int connectfd = connect(socketfd, (struct sockaddr *)&servaddr, addrlen); // send connection request to server, which the server will accept if successful
    if (connectfd == -1)
    {
        cout << "connect failed";
        exit(1);
    }

    int readfd = recv(socketfd, buff, sizeof(buff), 0);
    if (readfd == -1)
    {
        cout << "read failed";
        exit(1);
    }

    close(socketfd);
}