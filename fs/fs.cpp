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
#include <fstream>
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
        read_disk(im, 16);

        // Get Root directory and working directory
        read_disk(rd, initDataBlock);
        read_disk(wd, initDataBlock);

        // load users
        read_disk(curr_user, 17);

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
        for (int i = 0; i < 18 + TOTAL_INODE_NUM / 32; ++i)
        {
            bm.bmap[i] = '1';
        }
        write_to_disk(bm, sizeof(blockBitmap), 0);

        // Initialize inode bit map and write it to disk
        for (int i = 0; i < TOTAL_INODE_NUM; ++i)
        {
            im.imap[i] = '0';
        }

        write_to_disk(im, sizeof(iNodeBitmap), 16);

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

        write_to_disk(users, sizeof(User), 17);

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
    int inode_position = 18 * BLOCK_SIZE + inodeNum * 128;

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

    // Calculate inode position in Bytes
    // int inode_position = 2*BLOCK_SIZE + inodeNum/32*BLOCK_SIZE + inodeNum/32*128;
    int inode_position = 18 * BLOCK_SIZE + inodeNum * 128;
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
    write_to_disk(im, sizeof(iNodeBitmap), 16);
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

    terminate_File_System();

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

    terminate_File_System();

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

    terminate_File_System();

    return rc;
}

// use an inode and update its parameters accordingly
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
    initialize_inode(inode, 0, 0, 0, "0000", 0, 0, 0);
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

// copy a host file to the current directory in the FS
int FileSystem::my_lcp(char *host_file)
{
    int rc = -1;

    FILE *pFile = fopen(host_file, "r+b");

    if (pFile != NULL)
    {
        fseek(pFile, 0, SEEK_END);
        long len = ftell(pFile);
        file_size = len;
        fseek(pFile, 0, SEEK_SET);

        int num_of_blocks = ceil(float(len) / BLOCK_SIZE);
        
        Inode inode;
        initialize_inode(inode, 0, 0, BLOCK_SIZE, "1777", 1, 1, 1);
        int inodeNum = get_free_inode();
        rc = inodeNum;

        int file_name_len = strlen(host_file);
        for (int i = 0; i < 16; i++){
            if (wd.dirEntries[i].inodeNumber == -1){
                wd.dirEntries[i].inodeNumber = rc;
                
                if (file_name_len <= 250){
                    wd.dirEntries[i].name = strdup(host_file);
                }
                else {
                    perror("file name exceeded 250");
                    exit(1);
                }
                break;
            }
        }

        // update inode's file_size attribute
        inode.file_size = len;

        int temp_file_size = file_size;
        for (int i = 0; i < num_of_blocks; i++)
        {
            int write_size = (temp_file_size < BLOCK_SIZE) ? temp_file_size : BLOCK_SIZE;

            // Read the file in chunks
            char buffer[BLOCK_SIZE];
            memset(buffer, 0, BLOCK_SIZE);
            // fseek(pFile, i * BLOCK_SIZE, SEEK_SET);
            fread(buffer, 1, write_size, pFile);

            Block block;
            // strncpy(block.text, buffer, BLOCK_SIZE);
            memcpy(block.text, buffer, BLOCK_SIZE);

            // Get a free block and write the block to disk
            int blockNum = get_free_block();
            write_to_disk(block, sizeof(Block), blockNum);

            //save block numbers on the inode
            if (i < 12){
                inode.direct_block_pointers[i] = blockNum;
            }

            temp_file_size -= write_size;
        }

        updateInode(inode, inodeNum);

        fclose(pFile);
    }

    return rc;
}

// copy a FS file from the current directory to the current directory in the host system
int FileSystem::my_Lcp(char *fs_file)
{
    int rc = -1; // rc here is the inode number of fs_file

    // check if fs_file exists on disk
    for (int i = 0; i < 16; i++) {
        if (strcmp(wd.dirEntries[i].name, fs_file) == 0) {
            rc = wd.dirEntries[i].inodeNumber;
            break;
        }
    }

    if (rc != -1) {
        string str_file_name = "export_" + string(fs_file);
        // str_file_name += ".txt";

        FILE *pFile = fopen(str_file_name.c_str(), "a+b");

        Inode inode;
        readInode(inode, rc);

        int steps = ceil(float(file_size)/ 4096);

        for (int i = 0; i < 12 && i < steps; i++) {
            if (inode.direct_block_pointers[i] != 0) {
                Block block;
                read_disk(block, inode.direct_block_pointers[i]);

                int write_size = (file_size < BLOCK_SIZE) ? file_size : BLOCK_SIZE;

                // fseek(pFile, i * BLOCK_SIZE, SEEK_SET);
                fwrite(block.text, 1, write_size, pFile);

                file_size -= write_size;

                // Break the loop if we have copied all the required bytes
                if (file_size == 0) {
                    break;
                }
            }
        }


        fclose(pFile);
    }

    return rc;
}

//outputs contents of a directory/file
string FileSystem::my_cat(string file) {
    string rc = "";
    int inode_number = -1; // rc here is the inode number of fs_file

    // check if fs_file exists on disk
    const char* fs_file = file.c_str();
    for (int i = 0; i < 16; i++) {
        if (strcmp(wd.dirEntries[i].name, fs_file) == 0) {
            inode_number = wd.dirEntries[i].inodeNumber;
            break;
        }
    }

    if (inode_number != -1) {
        Inode inode;
        readInode(inode, inode_number);

        int steps = ceil(float(inode.file_size)/ 4096);

        // file is type directory
        if (inode.Mode[0] == '0'){
            rc = file + ": Is a directory";
        }
        else {
            for (int i = 0; i < 12 && i < steps; i++) {
                if (inode.direct_block_pointers[i] != 0) {
                    Block block;
                    read_disk(block, inode.direct_block_pointers[i]);

                    rc += block.text;
                }
            }
        }
    }

    else {
        rc = file + " not found";
    }

    return rc;
}

// create a hard link for files
int FileSystem::my_ln(string src_file, string dst_file){
    int rc = -1;

    // check if fs_file exists on disk
    const char* fs_file = src_file.c_str();
        for (int i = 0; i < 16; i++){
            if (strcmp(wd.dirEntries[i].name, fs_file) == 0){
                rc = wd.dirEntries[i].inodeNumber;
                break;
            }
        }

        if (rc != -1) {
            Inode og_inode;
            readInode(og_inode, rc);

            // file is type directory
            if (og_inode.Mode[0] == '0'){
                rc = -2;
            }
            else {
                // create new inode
                Inode new_inode;
                int new_inode_number = get_free_inode();

                // duplicate block numbers from the src_file inode
                readInode(new_inode, rc);

                // increment the link count of the original inode
                og_inode.link_count++;

                //update both inodes
                updateInode(og_inode, rc);
                updateInode(new_inode, new_inode_number);

                // add dst_file to the wd
                for (int i = 0; i < 16; i++){
                    if (wd.dirEntries[i].inodeNumber == -1){
                        strcpy(wd.dirEntries[i].name, dst_file.c_str()); // add name
                        wd.dirEntries[i].inodeNumber = new_inode_number;
                        break;
                    }
                }
            }
        }
    
    return rc;
}

// removes files
int FileSystem::my_rm(string file) {
    int rc = -1;
    int pos_in_wd = -1; // which index the file is in wd
    
    //check if the file exists in the FS
    const char* fs_file = file.c_str();
    for (int i = 0; i < 16; i++){
        if (strcmp(wd.dirEntries[i].name, fs_file) == 0) {
            rc = wd.dirEntries[i].inodeNumber;
            pos_in_wd = i;
            break;
        }
    }

    if (rc != -1){
        Inode inode;
        readInode(inode, rc);

        if (inode.link_count == 0){
            // clear the block data
            for (int i = 0; i < 12; i++){
                if (inode.direct_block_pointers[i] != 0){
                    Block block;
                    read_disk(block, inode.direct_block_pointers[i]);
                    strcpy(block.text, ""); // reset block data
                    write_to_disk(block, sizeof(Block), inode.direct_block_pointers[i]); // write back to disk
                    bm.bmap[inode.direct_block_pointers[i]] = '0'; // reset block bitmap
                }
            }
        }

        // update the inode data
        strcpy(wd.dirEntries[pos_in_wd].name, ""); // remove name
        wd.dirEntries[pos_in_wd].inodeNumber = -1; // reset inode_number
        initialize_inode(inode, 0, 0, 0, "", 0, 0, 0); // reset inode
        updateInode(inode, rc);
        im.imap[rc] = '0'; // reset inode bitmap
    }

    return rc;
}

// copy files
int FileSystem::my_cp(string src_file, string dst_file) {
    int rc = -1;

    //check if the file exists in the FS
    const char* fs_file = src_file.c_str();
    for (int i = 0; i < 16; i++){
        if (strcmp(wd.dirEntries[i].name, fs_file) == 0) {
            rc = wd.dirEntries[i].inodeNumber;
            break;
        }
    }

    if (rc != -1){
        Inode og_inode;
        readInode(og_inode, rc);
        
        // file is type directory
        if (og_inode.Mode[0] == '0'){
            rc = -2;
        }

        else {
            Inode new_inode;
            initialize_inode(new_inode, 0, 0, 0, "", 1, 1, 1); // reset inode
            for (int i = 0; i < 12; i++){
                if (og_inode.direct_block_pointers[i] != 0){
                    Block new_block;
                    int new_block_number = get_free_block();
                    read_disk(new_block, og_inode.direct_block_pointers[i]); // then new_block has the old block's data
                    write_to_disk(new_block, sizeof(Block), new_block_number); // write the copied block to a free block area
                    new_inode.direct_block_pointers[i] = new_block_number; // add the new block to the new inode
                }
            }

            // put the new inode in a free inode area
            int free_inode_num = get_free_inode();
            updateInode(new_inode, free_inode_num);

            // add dst_file to the wd
                for (int i = 0; i < 16; i++){
                    if (wd.dirEntries[i].inodeNumber == -1){
                        strcpy(wd.dirEntries[i].name, dst_file.c_str()); // add name
                        wd.dirEntries[i].inodeNumber = free_inode_num;
                        break;
                    }
                }

            
        }
    }

    return rc;
}

// just for testing
void FileSystem::ps()
{
    int a = my_lcp("file.txt");
    cout << ceil(float(file_size)/BLOCK_SIZE);
}

//******Server Side Code*******

// Starts the server
void FileSystem::start_server()
{
    // server connection
    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(2006);
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
    //only for functions that take in unlimited prompt
    // int prompt_length = 0;
    string str_param(parameter);      // convert char array to string
    // for (int i = 0; i < str_param.length(); i++){
    //     if (str_param[i] == ' '){
    //         prompt_length++;
    //     }
    // }
    // prompt_len = prompt_length;

    // prompt_files = new string[prompt_len];
    // int first_space = str_param.find(' ');
    // int k = 0;
    // for (int i = 0; i < str_param.length(); i++){
    //     if (str_param[i] == ' '){
    //         k++;
    //     }
    //     prompt_files[k] = str_param[i];
    // }

    ///////////


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

    // prompt_files = identify;

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
                rc = "Directory not created" + prompt[1];
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
        if (prompt[1] == "") {
            rc = "No argument, please try again";
        }
        else {
            prompt[1] = prompt[1].substr(1);

            char data[prompt[1].length()];
            strcpy(data, prompt[1].c_str());
            if (my_lcp(data) == -1){
                string s(data);
                rc = "File not found in host" + s;
            }
            else {
                rc = "File successfully written on disk";
            }
        }
    }

    else if (prompt[0] == "Lcp")
    {
        // rc = Lcp(prompt[1]);
        if (prompt[1] == "") {
            rc = "No argument, please try again";
        }
        else {
            prompt[1] = prompt[1].substr(1);

            char data[prompt[1].length()];
            strcpy(data, prompt[1].c_str());
            if (my_Lcp(data) == -1){
                string s(data);
                rc = "File not found in file system " + s;
            }
            else {
                rc = "File successfully written to " + prompt[1];
            }
        }
    }

    else if (prompt[0] == "cat"){
        //rc = cat(prompt[1]);
        prompt[1] = prompt[1].substr(1);
        rc = my_cat(prompt[1]);
    }

    else if (prompt[0] == "ln"){
        prompt[1] = prompt[1].substr(1);
        prompt[2] = prompt[2].substr(1);
        if (my_ln(prompt[1], prompt[2]) == -1){
            rc = prompt[1] + " or " + prompt[2] + "not found";
        }
        else if (my_ln(prompt[1], prompt[2]) > -1){
            rc = "Hard link between " + prompt[1] + " and " + prompt[2] + " created";
        }
    }

    else if (prompt[0] == "rm"){
        prompt[1] = prompt[1].substr(1);
        if (my_rm(prompt[1]) == -1){
            rc = "File not found";
        }
        else {
            rc = "File successfully removed";
        }
    }

    else if (prompt[0] == "cp"){
        prompt[1] = prompt[1].substr(1);
        prompt[2] = prompt[2].substr(1);
        if (my_ln(prompt[1], prompt[2]) == -1){
            rc = prompt[1] + " or " + prompt[2] + "not found";
        }
        else if (my_ln(prompt[1], prompt[2]) > -1){
            rc = "Copied " + prompt[1] + " to " + prompt[2];
        }
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
    read_disk(curr_user, 17);

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