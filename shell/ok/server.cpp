#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

int main() {
    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9004);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    int bindfd = bind(socketfd, (sockaddr *)&servaddr, sizeof(servaddr));
    int listenfd = listen(socketfd, 1);

    cout << "Listening for connections..." << endl;

    while (true) {
        socklen_t addrLen = sizeof(servaddr);
        int acceptfd = accept(socketfd, (sockaddr *)&servaddr, &addrLen);

        if (acceptfd == -1) {
            cout << "Connection failed" << endl;
            continue;
        }

        cout << "Connection accepted" << endl;

        while (true) {
            char readMsg[4000] = {};
            int readfd = read(acceptfd, readMsg, sizeof(readMsg));

            if (readfd <= 0) {
                // Client disconnected or encountered an error
                cout << "Client disconnected" << endl;
                close(acceptfd);
                break; // Exit the inner loop
            }

            cout << "Client: " << readMsg << endl;

            // Process the message and send a response
            // For simplicity, let's echo the message back to the client
            write(acceptfd, readMsg, readfd);
        }
    }

    // Close the server socket before exiting
    close(socketfd);

    return 0;
}
