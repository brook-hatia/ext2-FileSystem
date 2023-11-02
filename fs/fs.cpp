#include <iostream>
#include <string>
#include <fstream>
#include "fs.h"
#define BLOCKSIZE 4096

using namespace std;

FileSystem::FileSystem()
{
    // initialize block and inode bitmaps
    block_bitmap = new int[1024];
    inode_bitmap = new int[1024];

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

FileSystem::~FileSystem()
{
    delete[] block_bitmap;
    delete[] inode_bitmap;
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
        // read back the inode bitmap from disk
        int temp_bitmap[1024] = {0};

        // look for a free bit (bit = 0)
        for (int i = 0; i < 1024; i++)
        {
            if (temp_bitmap[i] == 0)
            {
                rc = i;
                temp_bitmap[rc] = 1;
                file.seekp(i * sizeof(int));
                file.write((char *)&temp_bitmap, sizeof(temp_bitmap));
                break;
            }
        }
    }

    return rc;
}

bool FileSystem::check_disk()
{
    fstream file("disk", ios::in); // read disk

    return file.good();
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
    in.block_pointers = new int[100]; // 61,917,364,224 block pointers (direct pointers + indirect pointers(single, double, triple))
    for (int i = 0; i < 100; i++)
    {
        in.block_pointers[i] = -1;
    }
}

void FileSystem::write_to_disk(std::string str)
{
    bool is_file = check_disk();

    fstream file("disk", ios::out | ios::trunc); // write on disk

    Block block;       // initialize block
    int block_pos = 0; // position of block on the disk

    // disk doesn't exist, so initialize the disk
    if (!is_file)
    {
        // write block_bitmap
        file.write((char *)&block_bitmap, sizeof(block_bitmap));

        // write inode_bitmap
        file.write((char *)&inode_bitmap, sizeof(inode_bitmap));

        // write inodes
        for (int i = 0; i < 100; i++)
        {
            Inode in;
            file.write((char *)&in, sizeof(in));
        }

        // write root ("0:0")
        strcpy(block.text, "0:0");
    }

    else
    {
        strcpy(block.text, str.c_str());
    }

    file.write((char *)&block, sizeof(block));

    block_pos = static_cast<int>(file.tellp()); // get current position of file on the disk

    // write address of block to a free inode
    int offset = inode_lookup();
    Inode temp_inode;
    initialize_inode(temp_inode);
    temp_inode.block_pointers[0] = block_pos;
    file.seekp(offset * sizeof(Inode));
    file.write((char *)&temp_inode, sizeof(temp_inode));
}