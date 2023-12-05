#include "fs.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

#include <sys/socket.h> //library for server-client communication
#include <netinet/in.h> //for serveaddr_in which is used for IPv4
#include <unistd.h>     //for close()

using namespace std;
int main()
{
    FileSystem fs;
    fs.initialize_File_System();
    //fs.ps();

    fs.start_server();



    return 0;
}