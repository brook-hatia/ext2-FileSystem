#include <iostream>
#include <string>
#include <fstream>
#include "fs.h"
#define BLOCKSIZE 4096

using namespace std;

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
    ifstream file;
    file.open("disk", ifstream::in);

    return file.good();
}

void FileSystem::write_to_disk(std::string str)
{
    int offset = inL->free_inode_lookup(); // return next free bit from inode bitmap
    cout << offset << endl;
    ofstream file;
    file.open("disk", ios_base::app);

    Block write_block;
    strcpy(write_block.text, str.c_str()); // copy prompt to block

    file.seekp(4096 * offset);

    file.write(reinterpret_cast<char *>(&write_block), sizeof(write_block)); // write block on "disk"
    file.close();

    // ******************** add blocks on inode ******************************

    this->curr->block_pointers[offset] = 4096 * offset; // save the block in the current inode
    // cout << this->curr->block_pointers[offset];
    this->curr->inode_num++;
    inL->create_inode(this->curr); // add inode to inode list
}

// read blocks via inodes
std::string FileSystem::read_from_disk()
{
    std::string rc = "";

    ifstream file;
    file.open("disk");

    for (int i = 0; i <= this->curr->inode_num; i++)
    {
        int offset = this->curr->block_pointers[i];

        // cout << offset << endl;
        file.seekg(offset); // Move the get pointer to the appropriate position

        Block read_block;
        file.read(reinterpret_cast<char *>(&read_block), sizeof(read_block)); // Read the block from the file

        // Output = " read_block.text "
        rc += " ";
        rc += read_block.text;
        rc += " ";
    }

    file.close(); // Close the file after reading

    return rc;
}