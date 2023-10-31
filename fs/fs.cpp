#include <iostream>
#include <string>
#include <fstream>
#include "fs.h"
#define BLOCKSIZE 4096

using namespace std;

Block::Block()
{
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        this->text[i] = 0;
    }
}

User::User()
{
    this->permission = 777; // root user
}

FileSystem::FileSystem()
{
    this->curr = new inode();
    this->inL = new inodeList();
}

FileSystem::~FileSystem()
{
    delete curr;
    delete inL;
}

// check if "disk" file exists
bool FileSystem::check_disk()
{
    ifstream file("disk", ifstream::in);

    return file.good();
}

void FileSystem::write_to_disk(std::string str)
{
    bool is_file = check_disk();

    fstream file("disk", ios::in | ios::out | ios::trunc);

    int block_pos = 0; // get position of block on disk

    if (!is_file)
    {
        // write bitmap on disk
        bi
        for (int i = 0; i < 1024; i++)
        {
            file.write((char *)&bitmap, sizeof(bitmap));
        }
        /********************************** I haven't written indirect block pointers. Finish that later ***************************/
        // write inodes to disk
        for (int i = 0; i < 1024; i++)
        {
            inode *new_inode = new inode();
            file.write((char *)&new_inode, sizeof(new_inode));
        }

        // write contents on disk
        Block write_block;
        strcpy(write_block.text, str.c_str());
        file.write((char *)&write_block, sizeof(write_block));
        block_pos = static_cast<int>(file.tellp());

        // write block position to a free inode
    }
}

// read blocks via inodes
std::string FileSystem::read_from_disk(std::string str)
{
    std::string rc = "";

    fstream file("disk", ios::in);

    file.seekg(1024 * 2);

    Block read_block;
    strcpy(read_block.text, " ");
    file.read((char *)&read_block, sizeof(read_block));

    rc = read_block.text;

    return rc;
}