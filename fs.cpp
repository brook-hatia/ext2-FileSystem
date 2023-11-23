#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>
#include <cmath>
#include <vector>
#include <sstream>
#include <sys/socket.h> //library for server-client communication
#include <netinet/in.h> //for serveaddr_in which is used for IPv4
#include <unistd.h>     //for close()
#include "fs.h"
using namespace std;

FileSystem::FileSystem()
{
}

FileSystem::~FileSystem()
{
}

// Reads data from disk and store in a structure
template <typename T>
void FileSystem::read_disk(T &x, int blockNum)
{
    // Open the file for reading
    FILE *pFile = fopen("disk", "rb");

    // Get the first block to read from
    fseek(pFile, blockNum * BLOCK_SIZE, SEEK_SET);
    // Initialize buffer to store data
    char read_buffer[BLOCK_SIZE];
    // Read the data from the file into a buffer
    fread(read_buffer, 1, BLOCK_SIZE, pFile);

    // Close the file after reading
    fclose(pFile);
    // Copy the data from the buffer into the struct
    memcpy(&x, read_buffer, sizeof(x));
}

// Writes a structure to disk, cant write array
template <typename T>
void FileSystem::write_to_disk(T x, int len, int blockNum)
{

    // Open File for updating
    FILE *pFile = fopen("disk", "r+b");

    // Initialize buffer to store data
    char buffer[len];

    // Setting buff as same byte size as node structure and fill with 0
    memset(buffer, 0, len);
    // Copy the data from the struct into the buff
    memcpy(buffer, &x, sizeof(x));

    // get starting of file move file pointer to the start
    fseek(pFile, blockNum * BLOCK_SIZE, SEEK_SET);
    // write the structure into disk
    fwrite(buffer, sizeof(char), sizeof(x), pFile);

    fclose(pFile);
}

// Initializes the File System
void FileSystem::initialize_File_System()
{
    // Instance Variables
    FILE *pFile;

    // Check if Disk Already exists if not initialize the disk
    if ((pFile = fopen("disk", "r")))
    {
        fclose(pFile);
        initDataBlock = 18 + TOTAL_INODE_NUM / 32;

        // Load Inode and Block Bitmap from disk
        read_disk(bm, 0);
        read_disk(im, 1);

        // Get Root directory and working directory
        read_disk(rd, initDataBlock);
        read_disk(wd, initDataBlock);

        // load users
        read_disk(curr_user, 2);

        atRoot = 1;
        currentDirectoryBlock = initDataBlock;
        cwd = "/";
    }

    else
    {
        atRoot = 1;
        pFile = fopen("disk", "wb");

        // initialize null buffer
        char buffer[BLOCK_SIZE];
        memset(buffer, 0, BLOCK_SIZE);

        // Using buffer to initialize whole disk as NULL
        for (int i = 0; i < TOTAL_BLOCK_NUM; ++i)
        {
            fwrite(buffer, 1, BLOCK_SIZE, pFile);
        }

        fclose(pFile);

        // Initialize block bit map and write it to disk
        for (int i = 0; i < TOTAL_BLOCK_NUM; ++i)
        {
            bm.bmap[i] = '0';
        }

        // Mark block used by the bit maps and inode
        for (int i = 0; i < 17 + TOTAL_INODE_NUM / 32; ++i)
        {
            bm.bmap[i] = '1';
        }
        write_to_disk(bm, sizeof(blockBitmap), 0);

        // Initialize inode bit map and write it to disk
        for (int i = 0; i < TOTAL_INODE_NUM; ++i)
        {
            im.imap[i] = '0';
        }

        write_to_disk(im, sizeof(iNodeBitmap), 1);

        cwd = "/";

        // create and write users on disk
        User users;

        users.name[0] = "root";
        users.uid[0] = 100;

        users.name[1] = "user1";
        users.uid[1] = 101;

        users.name[2] = "user2";
        users.uid[2] = 102;

        users.name[3] = "user3";
        users.uid[3] = 103;

        users.name[4] = "user4";
        users.uid[4] = 104;

        users.name[5] = "user5";
        users.uid[5] = 105;

        write_to_disk(users, sizeof(User), 2);

        // Initialize inodes;
        for (int i = 0; i < TOTAL_INODE_NUM; ++i)
        {
            inodeArray[i] = Inode();
            inodeArray[i].block_count = 0;
            inodeArray[i].uid = 0;
            inodeArray[i].gid = 0;
            inodeArray[i].indirect_block_address = -1;

            // Set the entire Mode array to 0
            for (int j = 0; j < 12; ++j)
            {
                inodeArray[i].Mode[j] = 0;
            }

            // Set the entire creation_time, modified_time, and read_time arrays to 0
            for (int j = 0; j < 14; ++j)
            {
                inodeArray[i].creation_time[j] = 0;
                inodeArray[i].modified_time[j] = 0;
                inodeArray[i].read_time[j] = 0;
            }

            // Set indirect_block_address and direct_block_pointers to an appropriate value (0)
            inodeArray[i].indirect_block_address = -1;
            for (int j = 0; j < 12; ++j)
            {
                inodeArray[i].direct_block_pointers[j] = 0;
            }
        }

        // Write the inodes to disk
        pFile = fopen("disk", "r+b");
        fseek(pFile, 18 * BLOCK_SIZE, SEEK_SET);
        int len = sizeof(inodeArray);
        char inode_buff[len];
        memset(inode_buff, 0, len);
        memcpy(inode_buff, inodeArray, len);
        fwrite(inode_buff, sizeof(char), len, pFile);
        fclose(pFile);

        initDataBlock = 18 + TOTAL_INODE_NUM / 32;
        currentDirectoryBlock = initDataBlock;
        // Initialize root directory and write to disk
        for (int i = 0; i < 16; ++i)
        {
            wd.dirEntries[i].inodeNumber = -1;
        }

        Inode rootInode;
        initialize_inode(rootInode, 0, 0, BLOCK_SIZE, "0777", 1, 1, 1);
        updateInode(rootInode, 0);
        write_to_disk(wd, sizeof(directory), initDataBlock);
    }
}

void FileSystem::readInode(Inode &i, int inodeNum)
{
    // Reopen the file for reading
    FILE *pFile = fopen("disk", "rb");

    // Calculate inode position in Bytes
    // int inode_position = 2*BLOCK_SIZE + inodeNum/32*BLOCK_SIZE + inodeNum/32*128;
    int inode_position = 17 * BLOCK_SIZE + inodeNum * 128;

    // get to the correct Inode position
    fseek(pFile, inode_position, SEEK_SET);

    int len = sizeof(struct Inode);
    // initialize buffer to store data
    char read_buffer[len];
    // Read the data from the file into a buffer
    fread(read_buffer, 1, len, pFile);

    // Close the file after reading
    fclose(pFile);
    // Copy the data from the buffer into the struct
    memcpy(&i, read_buffer, sizeof(i));
}

// Update a inode
void FileSystem::updateInode(Inode i, int inodeNum)
{

    FILE *pFile = fopen("disk", "r+b");

    int len = sizeof(struct Inode);
    // set buffer
    char buffer[len];
    // setting buff as same byte size as node structure and fill with 0
    memset(buffer, 0, len);
    // copy to test buff from test
    memcpy(buffer, &i, sizeof(i));

    // Calculatte inode position in Bytes
    // int inode_position = 2*BLOCK_SIZE + inodeNum/32*BLOCK_SIZE + inodeNum/32*128;
    int inode_position = 17 * BLOCK_SIZE + inodeNum * 128;
    // get to the correct Inode position
    fseek(pFile, inode_position, SEEK_SET);

    // write the test node structure into start of file
    fwrite(buffer, sizeof(char), sizeof(i), pFile);

    fclose(pFile);
}

// When File System Terminate stores everything back to disk
void FileSystem::terminate_File_System()
{
    write_to_disk(bm, sizeof(blockBitmap), 0);
    write_to_disk(im, sizeof(iNodeBitmap), 1);
}

// Gets a free inode and its number and mark bitmap
int FileSystem::get_free_inode()
{
    int rc = -1;

    // Loop through bitmap to find free inode
    for (int i = 0; i < TOTAL_INODE_NUM; i++)
    {
        if (im.imap[i] == '0')
        {
            rc = i;
            im.imap[rc] = '1';
            break;
        }
    }

    return rc;
}

// Get the first block number of free block
int FileSystem::get_free_block()
{
    int rc = -1;
    // Loop to find first free block
    for (int i = 0; i < TOTAL_BLOCK_NUM; ++i)
    {
        if (bm.bmap[i] == '0')
        {
            rc = i;
            bm.bmap[i] = '1';
            break;
        }
    }
    return rc;
}

// Get the first block number of the 8 consecutive free blocks
int FileSystem::get_eight_free_block()
{
    int rc = -1;
    int consecutiveBlocks = 0;
    // Loop to find * consecutive blocks
    for (int i = 0; i < TOTAL_BLOCK_NUM; ++i)
    {
        if (bm.bmap[i] == '0')
        {
            consecutiveBlocks++;
            if (consecutiveBlocks == 8)
            {
                // first block
                rc = i - 8;
                // Loop to mark the blocks
                for (i = 0; i < 8; i++)
                {
                    bm.bmap[rc + i] = '1';
                }
                break;
            }
        }
        else
        {
            consecutiveBlocks = 0;
        }
    }
    return rc;
}

// use a inode and update its parameters accordinly
void FileSystem::initialize_inode(Inode &inode, int uid, int linkCount, int fileSize, string mode, int creTime, int modTime, int reTime)
{
    if (uid != -1)
    {
        inode.uid = uid;
    }

    if (linkCount != -1)
    {
        inode.link_count = linkCount;
    }

    if (fileSize != -1)
    {
        inode.file_size = fileSize;
        // Get the number of blocks needed for the file or directory
        int blockCount = ceil(float(fileSize) / BLOCK_SIZE);
        inode.block_count = blockCount;

        // Allocate 1 block for the file
        inode.direct_block_pointers[0] = get_free_block();
        // If more block needed allocate 8 more and store in direct block pointer
        if (blockCount > 1)
        {
            inode.direct_block_pointers[1] = get_eight_free_block();
            for (int i = 2; i < 9; ++i)
            {
                inode.direct_block_pointers[i] = inode.direct_block_pointers[i - 1] + 1;
            }
            // If more block is needed
            if (blockCount > 9)
            {
                inode.direct_block_pointers[9] = get_eight_free_block();
                for (int i = 10; i < 13; ++i)
                {
                    inode.direct_block_pointers[i] = inode.direct_block_pointers[i - 1] + 1;
                }
                inode.indirect_block_address = get_eight_free_block();
                int indirectBlockAddress[1024];
                for (int i = 1; i <= 5; ++i)
                {
                    indirectBlockAddress[i] = inode.direct_block_pointers[12] + i;
                }

                // if more blocks are needed
                if (blockCount > 17)
                {
                    int flag = 6;
                    // Loop to allocate more blocks when needed
                    //!! Need condition where indirect Array is full;
                    while (blockCount - 17 > 0)
                    {
                        indirectBlockAddress[flag] = get_eight_free_block();
                        blockCount - 8;
                        for (int i = 0; i < 7; ++i)
                        {
                            flag++;
                            indirectBlockAddress[flag] = indirectBlockAddress[flag - 1] + 1;
                        }
                    }
                }
            }
        }
    }

    if (mode != "-1")
    {
        // Copy the string into the char array using strncpy
        strncpy(inode.Mode, mode.c_str(), sizeof(inode.Mode) - 1);
        inode.Mode[sizeof(inode.Mode) - 1] = '\0';
    }

    // Set the entire creation_time, modified_time, and read_time arrays
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm *localTime = std::localtime(&currentTime);

    if (creTime != -1)
    {
        std::strftime(inode.creation_time, sizeof(inode.creation_time), "%d%b%H:%M:%S", localTime);
    }
    if (reTime != -1)
    {
        std::strftime(inode.read_time, sizeof(inode.read_time), "%d%b%H:%M:%S", localTime);
    }
    if (modTime != -1)
    {
        std::strftime(inode.modified_time, sizeof(inode.modified_time), "%d%b%H:%M:%S", localTime);
    }
}

// Gets the directory and returns block number
int FileSystem::get_directory_block(directory &dir, int inodeNum)
{
    Inode inode;
    readInode(inode, inodeNum);
    int block_number = inode.direct_block_pointers[0];
    read_disk(dir, block_number);
    return block_number;
}

// Create a directory
int FileSystem::my_mkdir(string directoryName)
{
    int rc = -1;
    // initialize inode
    Inode inode;
    int inodeNum = get_free_inode();
    initialize_inode(inode, 0, 0, BLOCK_SIZE, "0777", 1, 1, 1);

    // update working directory
    for (int i = 0; i < 16; i++)
    {
        if (wd.dirEntries[i].inodeNumber == -1)
        {
            rc = 1;
            wd.dirEntries[i].inodeNumber = inodeNum;              // add inode number
            strcpy(wd.dirEntries[i].name, directoryName.c_str()); // add name
            break;
        }
    }

    updateInode(inode, inodeNum);
    directory new_dir;
    for (int i = 0; i < 16; ++i)
    {
        new_dir.dirEntries[i].inodeNumber = -1;
    }

    if (atRoot)
    {
        // write working directory to disk
        write_to_disk(wd, sizeof(directory), initDataBlock);
    }
    else
    {
        // write working directory to disk in the correct block
        write_to_disk(wd, sizeof(directory), currentDirectoryBlock);
    }

    write_to_disk(new_dir, sizeof(directory), inode.direct_block_pointers[0]);

    return rc;
}

int FileSystem::my_cd(string directoryName)
{
    int rc = 1;
    int nameFound = 0;

    // Parse the directory name
    istringstream ss(directoryName);
    string component;
    vector<string> components;

    while (std::getline(ss, component, '/'))
    {
        components.push_back(component);
    }

    // check if is absolute path
    if (directoryName[0] == '/')
    {
        atRoot = 0;
        // Remove the first empty element in the vector
        components.erase(components.begin());

        directory temp = rd;
        string tempcwd = "/";
        // loop through all the names starting from root
        for (const string &comp : components)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (temp.dirEntries[i].name == comp)
                {
                    nameFound = 1;
                    // Set working directory to that
                    tempcwd += comp + "/";
                    currentDirectoryBlock = get_directory_block(temp, temp.dirEntries[i].inodeNumber);
                    break;
                }
            }
            // If name not found break
            if (!nameFound)
            {
                rc = -1;
                nameFound = 1; // to mark difference of found
                break;
            }
            // Restart nameFound for next directory
            nameFound = 0;
        }
        // Set working directory to it if name is found
        if (!nameFound)
        {
            wd = temp;
            cwd = tempcwd;
        }
    }
    else
    {
        atRoot = 0;
        directory temp = wd;
        string tempcwd = cwd;
        // loop through all the names
        for (const string &comp : components)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (temp.dirEntries[i].name == comp)
                {
                    tempcwd += comp + "/";
                    nameFound = 1;
                    currentDirectoryBlock = get_directory_block(temp, temp.dirEntries[i].inodeNumber);
                    break;
                }
            }
            // If name not found break
            if (!nameFound)
            {
                rc = -1;
                nameFound = 1; // to mark difference of found
                break;
            }
            // Restart nameFound for next directory
            nameFound = 0;
        }

        // Set working directory to it if name is found
        if (!nameFound)
        {
            wd = temp;
            cwd = tempcwd;
        }
    }

    return rc;
}

int FileSystem::my_rmdir(string directoryName)
{
    int flag = 0;         // directory doesn't exist
    int inode_number = 0; // directoryName's inode number
    for (int i = 0; i < 16; i++)
    {
        if (wd.dirEntries[i].inodeNumber == -1)
        {
            break;
        }

        if (wd.dirEntries[i].name == directoryName)
        {
            flag = 1;                                    // directory exists
            inode_number = wd.dirEntries[i].inodeNumber; // get directoryName's inode number
            cout << wd.dirEntries[i].name;
            wd.dirEntries[i].inodeNumber = -1; // reset inodeNumber;
            break;
        }
    }

    // reset/initialize inode
    Inode inode;
    readInode(inode, inode_number);
    initialize_inode(inode, 0, 0, 0, "0000", 0, 0, 0); // inode is reset
    updateInode(inode, inode_number);

    // get block number
    directory dir;
    int block_number = get_directory_block(dir, inode_number);
    read_disk(dir, inode_number);

    // reset the directory entries
    for (int i = 0; i < 16; ++i)
    {
        dir.dirEntries[i].inodeNumber = -1;
    }

    // update block bitmap and inode bitmap
    bm.bmap[block_number] = '0';
    im.imap[inode_number] = '0';

    // Write working directory back to disk
    write_to_disk(dir, sizeof(directory), inode.direct_block_pointers[0]);
    write_to_disk(wd, sizeof(directory), initDataBlock);
    write_to_disk(bm, sizeof(blockBitmap), 0);
    write_to_disk(im, sizeof(iNodeBitmap), 1);

    // directory doesn't exist
    if (!flag)
    {
        cout << "Directory not found" << endl;
    }

    return flag;
}

string FileSystem::my_ls()
{
    string outPut = "";
    int flag = 1;
    // does equivalent of `ls -l`
    // format: {filetype}{permission bits} {dirEntries.size()} {owner} {root/staff?} {file size} {creation date} {dirEntries[i].name}
    for (int i = 0; i < 16; i++)
    {
        if (wd.dirEntries[i].inodeNumber == -1)
        {
            break;
        }
        flag = 0;
        Inode new_inode;
        readInode(new_inode, wd.dirEntries[i].inodeNumber);

        // process the mode. 0777

        char file_type;
        if (new_inode.Mode[0] == '0')
        {
            file_type = 'd';
        }
        else
        {
            file_type = '-';
        }

        string permission_bits;

        string temp(new_inode.Mode);
        for (int i = 1; i < temp.size(); i++)
        {
            if (temp[i] == '0')
            {
                permission_bits += "----";
            }
            if (temp[i] == '1')
            {
                permission_bits += "--x-";
            }
            if (temp[i] == '2')
            {
                permission_bits += "-w--";
            }

            if (temp[i] == '3')
            {
                permission_bits += "-wx-";
            }
            if (temp[i] == '4')
            {
                permission_bits += "r---";
            }
            if (temp[i] == '5')
            {
                permission_bits += "r-x-";
            }
            if (temp[i] == '6')
            {
                permission_bits += "rw--";
            }
            if (temp[i] == '7')
            {
                permission_bits += "rwx-";
            }
        }

        // read block
        directory new_dir;
        read_disk(new_dir, new_inode.direct_block_pointers[0]);
        int dir_entries_count = 0;

        // Loop to get number of file in subdirectory
        for (int j = 0; j < 16; j++)
        {
            if (new_dir.dirEntries[j].inodeNumber == -1)
            {
                break;
                i++;
            }
        }
        string owner = "0:0";
        string owner_class = "root";
        int file_size = new_inode.file_size;
        string file_creation_date = new_inode.creation_time;

        string dir_name = wd.dirEntries[i].name;
        outPut += file_type + permission_bits + " ";
        outPut += dir_entries_count + " " + owner + " " + owner_class + " " + file_creation_date + " " + dir_name + "\n";
    }
    if (flag)
    {
        outPut += "nothing here";
    }
    return outPut;
}

// Just for testing
void FileSystem::ps()
{
    read_disk(curr_user, 2);

    for (int i = 0; i < 6; i++)
    {
        cout << curr_user.name[i] << " " << curr_user.uid[i] << endl;
    }
}

//******Server Side Code*******

// Starts the server
void FileSystem::start_server()
{
    // server connection
    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(2005);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    int bindfd = bind(socketfd, (sockaddr *)&servaddr, sizeof(servaddr));
    int listenfd = listen(socketfd, 1);

    cout << "Listening for connections..." << endl;

    bool server_run = false;
    while (!server_run)
    {
        socklen_t addrLen = sizeof(servaddr);
        int acceptfd = accept(socketfd, (sockaddr *)&servaddr, &addrLen);

        if (acceptfd == -1)
        {
            cout << "Connection failed" << endl;
            continue;
        }

        cout << "Connection accepted" << endl;

        while (true)
        {
            char readMsg[4000] = {};
            int readfd = read(acceptfd, readMsg, sizeof(readMsg));

            if (readfd <= 0)
            {
                cout << "Client disconnected" << endl;
                close(acceptfd);
                break;
            }

            cout << "Client: " << readMsg << endl;

            if (strcmp(readMsg, "shutdown") == 0)
            {
                server_run = true; // Set the flag to true to break the outer loop
                break;             // Break the inner loop
            }

            // write(acceptfd, readMsg, readfd);
            cout << "Client: " << readMsg << endl;

            // scan the read message for function name, filename/path
            string *contents = scan(readMsg);
            // string sendMsg = cwd + " " + identify_function(contents);
            string sendMsg = identify_function(contents) + '.' + cwd;
            write(acceptfd, sendMsg.c_str(), (size_t)sendMsg.size()); // send message to client
        }
    }

    close(socketfd); // terminate client socket connection
}

// call appropriate functions from prompt
string *FileSystem::scan(char *parameter)
{
    string str_param(parameter);      // convert char array to string
    string *identify = new string[3]; // string[0] = function name, string[1] and string[2] = filenames/pathnames
    int j = 0;

    // user is not signed in
    if (str_param[0] == '0')
    {
        identify[0] = "sign_in";
        j = 1;
    }

    // user is signed in
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

// identify function names from filenames/paths
string FileSystem::identify_function(string *prompt)
{
    string rc;
    if (prompt[0] == "ls")
    {
        rc = my_ls();
    }

    else if (prompt[0] == "cd")
    {
        if (prompt[1] == "")
        {
            wd = rd;
            atRoot = 1;
            rc = "cd to root";
            currentDirectoryBlock = initDataBlock;
            cwd = "/";
        }
        else
        {
            if (my_cd(prompt[1]) == -1)
            {
                rc = "cd failed";
            }
            else
            {
                rc = "cd successful to " + prompt[1];
            }
        }
    }

    else if (prompt[0] == "mkdir")
    {
        if (prompt[1] == "")
        {
            rc = "No argument, please try again";
        }
        else
        {
            if (my_mkdir(prompt[1]) == -1)
            {
                rc = "Directory not created";
            }
            else
            {
                rc = "Directory created successfully " + prompt[1];
            }
        }
    }

    else if (prompt[0] == "rmdir")
    {
        if (prompt[1] == "")
        {
            rc = "No argument, please try again";
        }
        else
        {
            if (my_rmdir(prompt[1]) == 0)
            {
                rc = "Directory not found";
            }
            else
            {
                rc = "Directory " + prompt[1] + " removed successfully";
            }
        }
    }

    else if (prompt[0] == "lcp")
    {
        // rc = lcp(prompt[1]);
    }

    else if (prompt[0] == "Lcp")
    {
        // rc = Lcp(prompt[1]);
    }

    else if (prompt[0] == "shutdown")
    {
        // rc = shutdown();
    }

    else if (prompt[0] == "exit")
    {
        // rc = exit();
    }

    else if (prompt[0] == "sign_in")
    {
        if (sign_in(prompt[1]) == -1)
        {
            prompt[1] = prompt[1].substr(1);
            rc = "User " + prompt[1] + " not found";
        }
        else
        {
            prompt[1] = prompt[1].substr(1);
            rc = "1" + prompt[1]; // user found, send message with initial "1" which indicates user is signed in
        }
    }

    else if (prompt[0] == "whoami")
    {
        rc = who_am_i();
    }

    else
    {
        rc = "command not found";
    }

    return rc;
}

int FileSystem::sign_in(string name)
{
    int rc = -1;

    // read users
    read_disk(curr_user, 2);

    for (int i = 0; i < 6; i++)
    {
        string tempstr = '0' + curr_user.name[i];
        if (tempstr == name)
        {
            rc = 1;           // user exists on the disk
            current_user = i; // save the index of the user
            break;
        }
    }

    return rc;
}

string FileSystem::who_am_i()
{
    return curr_user.name[current_user];
}

void my_chown(string uName, string fName) {
	
	Inode* file = NULL; // initialize i-node pointer to NULL
	int uidOld = NULL; // initialize current owner UID to NULL
	
	// linear search working directory for i-node with filename 'fName' and set i-node pointer 'file' to its address
	for (int i = 0; i < 16; i++) {
		if (wd.dirEntries[i].name == fName) {
			file = &wd.dirEntries[i];
			uidOld = wd.dirEntries[i].uid;
			break;
		}
	}


	int uidNew = NULL; // initialize new owner UID to NULL
	
	// linear search users for user with name 'uName' and set uidNew to its UID
	for (int i = 0; i < 6; i++) {
		if (users.name[i] == uName) {
			uidNew = users.uid[i];
			break;
		}
	}
	
	// if fName and uName correspond to existing entities and current user has permission, change the owner of the specified file to the specified user
	if (file != NULL) {	
		if (curr_user.uid == uidOld or curr_user.uid == 100) { // check if current user is file owner or root user
			if (curr_user.uid != 100 and file.Mode[1] != 7) { // if current user is not root user, check if file owner does not have rwx permissions
				std::cout << "Permission denied"; // permission denied, throw error
			} else {
				initialize_inode(file, uidNew); // update the existing inode by initializing it with the uid tied to the username parameter
			}
		} else if (uidNew == NULL) { 
			std::cout << "User not found"; // user with name uName does not exist, throw error
		} else {
			std::cout << "Permission denied"; // permission denied, throw error
		}
	} else {
		std::cout << "File not found"; // file with name fName does not exist, throw error
	}
}