#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>
#include <cmath>
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
    memcpy(buffer, &x, len);

    // get starting of file move file pointer to the start
    fseek(pFile, blockNum * BLOCK_SIZE, SEEK_SET);
    // write the structure into disk
    fwrite(buffer, sizeof(char), len, pFile);

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
        initDataBlock = 2 + TOTAL_INODE_NUM / 32;
        // Load Inode and Block Bitmap from disk
        read_disk(bm, 0);
        read_disk(im, 1);
        read_disk(wd, initDataBlock);
    }

    else
    {
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
        for (int i = 0; i < 2 + TOTAL_INODE_NUM / 32; ++i)
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
        fseek(pFile, 2 * BLOCK_SIZE, SEEK_SET);
        int len = sizeof(inodeArray);
        char inode_buff[len];
        memset(inode_buff, 0, len);
        memcpy(inode_buff, inodeArray, len);
        fwrite(inode_buff, sizeof(char), len, pFile);
        fclose(pFile);

        initDataBlock = 2 + TOTAL_INODE_NUM / 32;

        // Initialize root directory and write to disk
        for (int i = 0; i < 16; ++i)
        {
            wd.dirEntries[i].inodeNumber = -1;
        }

        // wd.dirEntries[0].inodeNumber = 10;
        // wd.dirEntries[0].name[0] = 'o';
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
    int inode_position = 2 * BLOCK_SIZE + inodeNum * 128;

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
    int inode_position = 2 * BLOCK_SIZE + inodeNum * 128;
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

// use a inode and update its parameters accordingly
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
                //  Still working on it
                if (blockCount > 17)
                {
                    indirectBlockAddress;
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

    // // update inode
    // int blockNum = get_free_block();
    // for (int i = 0; i < 12; i++)
    // {
    //     if (inode.direct_block_pointers[i] == 0)
    //     {
    //         inode.direct_block_pointers[i] = blockNum;
    //         break;
    //     }
    // }
    updateInode(inode, inodeNum);
    directory new_dir;
    for (int i = 0; i < 16; ++i)
    {
        new_dir.dirEntries[i].inodeNumber = -1;
    }

    // write working directory to disk
    write_to_disk(wd, sizeof(directory), initDataBlock);
    write_to_disk(new_dir, sizeof(directory), inode.direct_block_pointers[0]);

    return rc;
}

void FileSystem::my_cd(string filename)
{
}

void FileSystem::my_ls()
{
    // does equivalent of `ls -l`
    // format: {filetype}{permission bits} {dirEntries.size()} {owner} {root/staff?} {file size} {creation date} {irEntries[i].name}
    // for (int i = 0; i < 16; i++){
    //     cout <<
    // }
}

// Just for testing
void FileSystem::ps()
{
    // Inode test;
    // readInode(test, 0);
    // directory test2;
    // read_disk(test2, test.direct_block_pointers[0]);
    // cout << test.direct_block_pointers[0] << endl;
    // cout << test2.dirEntries[0].inodeNumber << endl;
    // cout << test2.dirEntries[0].name[0] << endl;
    // my_mkdir("f1");
    // my_mkdir("f2");
    // my_mkdir("file 3");
    // my_mkdir("file 4");
    // my_mkdir("file 5");
    // my_mkdir("file 6");
    // my_mkdir("file 7");
    // my_mkdir("file 8");
    // my_mkdir("file 9");
    // my_mkdir("file 10");

    for (int i = 0; i < 16; i++)
    {
        if (wd.dirEntries[i].inodeNumber == -1)
        {
            break;
        }

        string str(wd.dirEntries[i].name);
        cout << str << endl;
    }
}

// call appropriate functions from prompt
string *FileSystem::scan(char *parameter)
{
    string str_param(parameter);      // convert char array to string
    string *identify = new string[3]; // string[0] = function name, string[1] and string[2] = filenames/pathnames
    int j = 0;

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
        // rc = my_ls();
    }

    else if (prompt[0] == "cd")
    {
        // rc = my_cd(prompt[1]);
    }

    else if (prompt[0] == "mkdir")
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

    else
    {
        rc = "command not found";
    }

    return rc;
}