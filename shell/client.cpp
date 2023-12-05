#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

int main()
{
    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    int connectfd = connect(socketfd, (sockaddr *)&servaddr, sizeof(servaddr));

    if (socketfd == -1 || connectfd == -1)
    {
        cout << "connection failed" << endl;
        return 1;
    }

    cout << "connection successful" << endl;
    string cwd = "";

    while (true)
    {
        cout << cwd << "->";

        string sendMsg;
        getline(cin, sendMsg);

        if (sendMsg == "shutdown")
        {
            // Send the shutdown message to the server
            write(socketfd, sendMsg.c_str(), sendMsg.size());
            // Client closes its own socket and exits the loop
            close(socketfd);
            break;
        }

        else if (sendMsg == "exit")
        {
            close(socketfd);
            break;
        }

        else if (sendMsg == "clear")
        {
            system("clear"); // clears client shell
        }

        // Send message to the server
        write(socketfd, sendMsg.c_str(), sendMsg.size());

        // Receive and display the server's response
        char readMsg[65535] = {};
        int readfd = read(socketfd, readMsg, sizeof(readMsg));
        cout << "Server: " << readMsg;

        // string s(readMsg);

        // int begin = 0;
        // for (int i = 0; i < s.size(); i++)
        // {
        //     if (s[i] == '.')
        //     {
        //         begin = i + 1;
        //         break; // '.' found, exit loop
        //     }
        // }

        // for (int i = begin; i < s.size(); i++)
        // {
        //     cwd += s[i];
        // }

        // std::cout << "cwd: " << cwd << std::endl;
    }

    close(socketfd); // Close the client socket before exiting
}