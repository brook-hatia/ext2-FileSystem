#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include "fs.h"
#include <stdio.h>
#include <string.h>

#define BLOCKSIZE 4096
#define BLOCKNUMBER 20

using namespace std;

FileSystem::FileSystem()
{
    // initialize block and inode bitmaps
    for (int i = 0; i < 10; i++)
    {
        block_bitmap[i] = 0;
        inode_bitmap[i] = 0;
    }

    // initialize inode
    Inode in;
    in.Mode = "";
    in.block_count = 0;
    in.link_count = 0;
    in.uid = 0;
    in.gid = 0;
    in.file_size = 0;
    in.creation_time = "";
    in.modified_time = "";
    in.read_time = "";

    for (int i = 0; i < 100; i++)
    {
        in.block_pointers[i] = -1; // 61,917,364,224 block pointers (direct pointers + indirect pointers(single, double, triple))
    }

    // initialize inodelist
    for (int i = 0; i < 10; i++)
    {
        inodeList[i] = in;
    }

    // initialize block
    Block bk;
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        bk.text[i] = ' ';
    }

    // initialize user
    User usr;
    usr.permission = "";
}

int FileSystem::inode_lookup()
{
    int rc = -1;

    fstream file("disk", ios::in | ios::out); // read/write on disk
    if (!file.is_open())
    {
        cout << "Couldn't open file";
    }
    else
    {
        // Read the inode bitmap from disk
        int temp_bitmap[10] = {0};
        file.seekg(10 * sizeof(int)); // Seek to the beginning of the inode bitmap
        file.read(reinterpret_cast<char *>(temp_bitmap), 10 * sizeof(int));

        // Look for a free bit (bit = 0)
        for (int i = 0; i < 10; i++)
        {
            if (temp_bitmap[i] == 0)
            {
                rc = i;
                temp_bitmap[rc] = 1;
                file.seekp((i + 10) * sizeof(int));                                  // Seek to the position in the file
                file.write(reinterpret_cast<char *>(&temp_bitmap[rc]), sizeof(int)); // Write the updated value back to the file
                break;
            }
        }
    }

    file.close(); // Close the file

    return rc;
}

bool FileSystem::check_disk()
{
    ifstream in("disk"); // read disk
    return in.good();
}

void FileSystem::initialize_inode(Inode &in)
{
    in.Mode = "";
    in.block_count = 0;
    in.link_count = 0;
    in.uid = 0;
    in.gid = 0;
    in.file_size = 0;
    in.creation_time = "";
    in.modified_time = "";
    in.read_time = "";

    for (int i = 0; i < 100; i++)
    {
        in.block_pointers[i] = -1; // 61,917,364,224 block pointers (direct pointers + indirect pointers(single, double, triple))
    }
}

void FileSystem::initialize_File_System()
{
    ofstream out("disk", ios::binary); // Open the file in binary mode

    // Write inode bitmap
    out.write(reinterpret_cast<char *>(inode_bitmap), 10 * sizeof(int));

    // Write block bitmap
    out.write(reinterpret_cast<char *>(block_bitmap), 10 * sizeof(int));

    // Write inodes
    Inode inode;
    initialize_inode(inode);
    for (int i = 0; i < 10; i++)
    {
        out.write(reinterpret_cast<char *>(&inode), sizeof(Inode));
    }

    // Write block to disk
    Block block;
    strcpy(block.text, "0:0");
    out.write(reinterpret_cast<char *>(&block), sizeof(Block));

    out.close();
}

void FileSystem::write_to_disk(std::string str)
{
    bool is_file = check_disk();

    if (!is_file)
    {
        initialize_File_System();
    }

    fstream out("disk", ios::app | ios::binary); // write the file on the disk

    // write block
    Block block;
    strcpy(block.text, str.c_str());
    out.write(reinterpret_cast<char *>(&block), sizeof(Block));
    int block_pos = static_cast<int>(out.tellp()); // get current position of the block on the disk

    // initialize inode
    Inode inode;
    initialize_inode(inode);

    int offset = inode_lookup();

    out.seekp(offset * sizeof(Inode));

    for (int i = 0; i < 100; i++)
    {
        if (inode.block_pointers[i] == -1)
        {
            inode.block_pointers[i] = block_pos; // put the position in the inode's block_pointer
        }
    }

    out.write(reinterpret_cast<char *>(&inode), sizeof(Inode)); // write the inode
}

void FileSystem::read_from_disk(std::string str)
{
    ifstream in("disk", ios::binary); // read file from disk

    // Read block bitmap
    in.read(reinterpret_cast<char *>(block_bitmap), 10 * sizeof(int));

    // Read inode bitmap
    in.read(reinterpret_cast<char *>(inode_bitmap), 10 * sizeof(int));

    // Read inodes
    for (int i = 0; i < 10; i++)
    {
        in.read(reinterpret_cast<char *>(&inodeList[i]), sizeof(Inode));
    }

    in.close();

    // Print block bitmap
    for (int i = 0; i < 10; i++)
    {
        cout << block_bitmap[i];
    }

    cout << endl;

    // Print inode bitmap
    for (int i = 0; i < 10; i++)
    {
        cout << inode_bitmap[i];
    }

    cout << endl;

    // within the inodelist
    // search each inode's block-pointer[i]
    // check if block.text at the ith position on disk matches with str
    // Inode temp_inode;
    // initialize_inode(temp_inode);

    // for (int i = 0; i < 10; i++)
    // {
    //     temp_inode = inodeList[i];
    //     for (int j = 0; j < 100; i++)
    //     {
    //         if (temp_inode.block_pointers[j] != -1)
    //         {
    //             in.seekg(i * sizeof(Inode));
    //             in.read(reinterpret_cast<char *>(&temp_inode), sizeof(Inode)); // Write the updated value back to the file
    //         }
    //     }
    // }
}