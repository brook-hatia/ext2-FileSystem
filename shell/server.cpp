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
    servaddr.sin_port = 8080;              // port number. "host to network short"
    servaddr.sin_addr.s_addr = INADDR_ANY; // bind to any available address

    // for server we need socket(), bind(), listen(), accept(), read(), write() function calls

    int socketfd = socket(AF_INET, SOCK_STREAM, 0); // create socket. AF_INET for ipv4, SOCK_STREAM for TCP connection, 0 for protocol

    int bindfd = bind(socketfd, (sockaddr *)&servaddr, (socklen_t)sizeof(servaddr)); // bind to specific IP addr. servaddr is the specified IP addr, and the socket length of the servaddr in bytes

    int listenfd = listen(socketfd, 1); // listen to incoming connection. 1 is the backlog of incoming message in queue
    cout << "listening to connection ..." << endl;

    socklen_t servaddrLen = sizeof(servaddr);                                    // size of servaddr in bytes
    int acceptfd = accept(socketfd, (struct sockaddr *)&servaddr, &servaddrLen); // new socket after connect() is called on client side

    if (socketfd == -1 || bindfd == -1 || listenfd == -1 || acceptfd == -1)
    { // error returns -1
        cout << "connection failed" << endl;
        return 1;
    }
    cout << "connection success" << endl;

    char sendMsg[100]; // allocate space for send message

    while (true)
    {
        char readMsg[3000] = {0};                                      // allocated space for receiving from client
        int readfd = read(acceptfd, readMsg, (size_t)sizeof(readMsg)); // receive message from client
        cout << "\nClient: " << readMsg << endl;

        cout << "Server shell: ";
        cin >> sendMsg;
        string str = sendMsg;
        if (str == "exit")
        {
            break;
        }

        int writefd = write(acceptfd, sendMsg, (size_t)sizeof(sendMsg)); // send message to client
        // cout << "\nwrite successful" << sendMsg;
    }

    close(socketfd); // close client's socket
}