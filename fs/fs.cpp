#include <iostream>
#include <string>
#include "fs.h"
#define BLOCKSIZE 4096

using namespace std;

User::User()
{
    this->permission = 777; // root user
}

FileSystem::FileSystem()
{
    this->prev = NULL;
    this->curr = NULL;
    this->inL = new inodeList();
}

FileSystem::~FileSystem()
{
    delete prev;
    delete curr;
    delete inL;
}
void FileSystem::write_to_disk(std::string str, inode *in)
{
    int offset = inL->free_inode_lookup(); // return next free bit from inode bitmap
    FILE *pFile = fopen("disk", "ab");     // append mode to avoid overwrite
    if (pFile == NULL)
    {
        perror("Error opening the file for reading");
        return;
    }

    // ******************** add blocks to disk ******************************

    Block write_block;
    strcpy(write_block.text, str.c_str()); // copy prompt to block

    int len = sizeof(write_block); // get size of struct

    char test_buff[len]; // set buffer

    memset(test_buff, 0, len); // setting buff as same byte size as node structure and fill with 0

    memcpy(test_buff, &write_block, sizeof(write_block)); // copy to test buff from test

    fseek(pFile, 4096 * offset, SEEK_SET);                       // move pointer to the next 8 block section
    fwrite(test_buff, sizeof(char), sizeof(write_block), pFile); // write the test node structure into start of file
    fclose(pFile);

    // ******************** add blocks on inode ******************************

    if ((this->prev == NULL && this->curr == NULL) || this->prev == this->curr)
    {
        in->block_pointers[offset] = 4096 * offset; // save the block in the existing inode
    }

    else
    {
        inode *new_inode = new inode();                    // create new inode
        new_inode->block_pointers[offset] = 4096 * offset; // add block to the new inode
        this->curr = new_inode;                            // curr points to the current directory or file
        this->prev = this->curr;                           // prev points to curr which means all files created will be on same directory
        inL->create_inode(new_inode);                      // add new inode to inode list and mark inode bitmap as 1
    }

    // ******************** change block bitmap ******************************
}

std::string FileSystem::read_from_disk()
{
    // int offset = inodeList::free_inode_lookup(); // return next free bit from inode bitmap
    std::string rc = "";
    FILE *pFile = fopen("disk", "rb"); // open file to read

    for (int i = 0; i < 12; i++)
    {
        int offset = this->curr->block_pointers[i];

        char read_buffer[BLOCKSIZE] = {0}; // initialize buffer to store data

        fseek(pFile, 4096 * i, SEEK_SET); // get starting of file move file pointer to the start

        fread(read_buffer, 1, BLOCKSIZE, pFile); // Read the data from the file into a buffer

        struct Block read_block; // initialize new block to store read data

        memcpy(&read_block, read_buffer, sizeof(read_block)); // Copy the data from the buffer into the struct

        // output = " read_block.text "
        rc += " ";
        rc += read_block.text;
        rc += " ";
    }
    fclose(pFile); // Close the file after reading

    // printf("Text: %s\n", block->text); // Now you can access the data in the 'read_test' structure
    return rc;
}