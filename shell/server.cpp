#include <iostream>
#include <sys/socket.h> //library for server-client communication
#include <netinet/in.h> //for serveaddr_in which is used for IPv4
#include <arpa/inet.h>
#include <unistd.h> //for close()
using namespace std;

int main()
{
    struct sockaddr_in servaddr; // the "_in" in sockaddr_in is IPv4 socket address structure

    // initialize servaddr
    servaddr.sin_family = AF_INET;         // IPv4
    servaddr.sin_port = 8080;              // port number. "host to network short"
    servaddr.sin_addr.s_addr = INADDR_ANY; // bind to any available address

    socklen_t addrlen = sizeof(servaddr); // size of the address
    char rcvMsg[1024] = {0};              // allocated space for receiving from client
    const char *sendMsg = "Hello world";  // message to send to client

    // for server we need socket(), bind(), listen(), accept(), read(), write() function calls to connect with client

    int socketfd = socket(AF_INET, SOCK_STREAM, 0); // create socket. AF_INET for ipv4, SOCK_STREAM for TCP connection, 0 for protocol
    if (socketfd == -1)
    {
        cout << "\nsocket creation failed";
        exit(1);
    }
    cout << "\nsocket created";

    int bindfd = bind(socketfd, (sockaddr *)&servaddr, addrlen); // bind to specific IP addr. servaddr is the specified IP addr, and addrlen is the length of the IP addr
    if (bindfd == -1)
    {
        cout << "\nbind failed";
        exit(1);
    }
    cout << "\nbind successful";

    int listenfd = listen(socketfd, 5); // listen to incoming connection. 5 is the backlog of incoming message in queue
    if (listenfd == -1)
    {
        cout << "\nlisten failed";
        exit(1);
    }
    cout << "\nlisten successful";

    int acceptfd = accept(socketfd, (struct sockaddr *)&servaddr, &addrlen); // new socket after connect() is called on client side
    if (acceptfd == -1)
    {
        cout << "\naccept failed";
        exit(1);
    }
    cout << "\naccept successful";

    int writefd = write(acceptfd, sendMsg, strlen(sendMsg)); // send message to client
    if (writefd == -1)
    {
        cout << "\nwrite failed";
        exit(1);
    }
    cout << "\nwrite successful" << rcvMsg;

    int readfd = read(acceptfd, rcvMsg, sizeof(rcvMsg)); // receive message from client
    if (readfd == -1)
    {
        cout << "\nread failed";
        exit(1);
    }
    cout << "\nread successful" << rcvMsg;
    close(socketfd); // close client's socket
}