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
    servaddr.sin_addr.s_addr = INADDR_ANY; // bind to server's IP addr

    // for client we need socket(), connect(), read(), write() function calls

    int socketfd = socket(AF_INET, SOCK_STREAM, 0); // create socket. AF_INET for ipv4, SOCK_STREAM for TCP connection, 0 for protocol

    int connectfd = connect(socketfd, (struct sockaddr *)&servaddr, (socklen_t)sizeof(servaddr)); // send connection request to server, which the server will accept if successful

    if (socketfd == -1 || connectfd == -1) // error returns -1
    {
        cout << "connection failed" << endl;
        return 1;
    }
    cout << "connection successful" << endl;

    while (true)
    {
        char *sendMsg = new char[3000]; // allocate space for send message

        cout << "Client shell: ";
        cin >> sendMsg;       // client put in command
        string str = sendMsg; // convert character array to string
        if (str == "exit")    // if client types "exit" terminate connection with server
        {
            break;
        }

        int writefd = write(socketfd, sendMsg, (size_t)sizeof(sendMsg)); // send message to server
        // cout << "\nwrite successful" << sendMsg;

        char *readMsg = new char[3000];                                // allocated space for receiving from server
        int readfd = read(socketfd, readMsg, (size_t)sizeof(readMsg)); // receive message from server
        cout << "\nServer: " << readMsg << "\n";                       // read message from server
    }

    close(socketfd);
}