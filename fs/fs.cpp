#include <stdio.h>
#include <string.h>
#include <iostream>
#include "fs.h"

using namespace std;

FileSystem::FileSystem(){
    
}

FileSystem::~FileSystem(){
    
}

//Writes a structure to disk, cant write array
int FileSystem::read_disk(auto &x, int blockNum){
     // Reopen the file for reading
    FILE *pFile = fopen("disk", "rb");

    if (pFile == NULL) {
        perror("Error opening the file for reading");
        return 0;
    }

    //get starting of file move file pointer to the start
    fseek(pFile, blockNum * BLOCK_SIZE, SEEK_SET);
           //initialize buffer to store data
    char read_buffer[BLOCK_SIZE];
    // Read the data from the file into a buffer
    fread(read_buffer, 1, BLOCK_SIZE, pFile);

    // Close the file after reading
    fclose(pFile);
    // Copy the data from the buffer into the struct
    memcpy(&x, read_buffer, sizeof(x));

    return 1;
}


void FileSystem::write_to_disk(auto x, int len, int blockNum){
    //get size of struct
    
    //set buffer
    char buffer[len];

    FILE *pFile = fopen ("disk", "r+b");

    //setting buff as same byte size as node structure and fill with 0
    memset(buffer, 0, len);
    //copy to test buff from test
    memcpy(buffer, &x, sizeof(x));

    //get starting of file move file pointer to the start
    fseek(pFile, blockNum * BLOCK_SIZE, SEEK_SET);
    //write the test node structure into start of file
    fwrite(buffer, sizeof(char), sizeof(x), pFile);

    fclose(pFile);

}

void FileSystem::initialize_File_System(){
    
    char buffer[BLOCK_SIZE];
    FILE *pFile = fopen ("disk", "wb");
    memset(buffer, 0, BLOCK_SIZE); // intialize null buffer


    //Using buffer to initialize whole disk as NULL
    for (int i = 0; i < TOTAL_BLOCK_NUM; ++i){
        fwrite(buffer, 1, BLOCK_SIZE, pFile);
    }

    fclose(pFile);

    //Initialize block bit map
    blockBitmap bm;
    for (int i = 0; i < TOTAL_BLOCK_NUM; ++i){
        bm.bmap[i] = '0';
    }
    write_to_disk(bm, sizeof(blockBitmap), 0);

    //Initialize inode bit map
    iNodeBitmap im;
    for (int i = 0; i < TOTAL_INODE_NUM; ++i){
        im.imap[i] = '1';
    }
    write_to_disk(im, sizeof(iNodeBitmap), 1);

    //Initialize inodes;
    for(int i = 0; i<TOTAL_INODE_NUM; ++i){
        inodeArray[i] = Inode();
        inodeArray[i].block_count = 3;
        inodeArray[i].uid = 0;
        inodeArray[i].gid = 0;
        inodeArray[i].indirect_block_address = -1;

        // Set the entire Mode array to 0
        for (int j = 0; j < 12; ++j) {
            inodeArray[i].Mode[j] = 0;
        }

        // Set the entire creation_time, modified_time, and read_time arrays to 0
        for (int j = 0; j < 14; ++j) {
            inodeArray[i].creation_time[j] = 0;
            inodeArray[i].modified_time[j] = 0;
            inodeArray[i].read_time[j] = 0;
        }

        // Set indirect_block_address and direct_block_pointers to an appropriate value (0)
        inodeArray[i].indirect_block_address = -1;
        for (int j = 0; j < 12; ++j) {
            inodeArray[i].direct_block_pointers[j] = 0;
        }
    }

    //Write the inodes to disk
    pFile = fopen ("disk", "r+b");
    fseek(pFile, 2*BLOCK_SIZE, SEEK_SET);
    int len = sizeof(inodeArray);
    char inode_buff[len];
    memset(inode_buff, 0, len);
    memcpy(inode_buff, inodeArray, len);
    fwrite(inode_buff, sizeof(char), len, pFile);

    fclose(pFile);
}

void FileSystem::readInode(Inode &i, int inodeNum){
         // Reopen the file for reading
    FILE *pFile = fopen("disk", "rb");

    //Calculatte inode position in Bytes
    //int inode_position = 2*BLOCK_SIZE + inodeNum/32*BLOCK_SIZE + inodeNum/32*128;
    int inode_position = 2*BLOCK_SIZE + inodeNum*128;

    //get to the correct Inode position
    fseek(pFile, inode_position, SEEK_SET);

    int len = sizeof(struct Inode);
    //initialize buffer to store data
    char read_buffer[len];
    // Read the data from the file into a buffer
    fread(read_buffer, 1, len, pFile);

    // Close the file after reading
    fclose(pFile);
    // Copy the data from the buffer into the struct
    memcpy(&i, read_buffer, sizeof(i));

}

void FileSystem::updateInode(Inode i, int inodeNum){

    FILE *pFile = fopen("disk", "r+b");

    int len = sizeof(struct Inode);
    //set buffer
    char buffer[len];
    //setting buff as same byte size as node structure and fill with 0
    memset(buffer, 0, len);
    //copy to test buff from test
    memcpy(buffer, &i, sizeof(i));

    //Calculatte inode position in Bytes
    //int inode_position = 2*BLOCK_SIZE + inodeNum/32*BLOCK_SIZE + inodeNum/32*128;
    int inode_position = 2*BLOCK_SIZE + inodeNum*128;
    //get to the correct Inode position
    fseek(pFile, inode_position, SEEK_SET);

    //write the test node structure into start of file
    fwrite(buffer, sizeof(char), sizeof(i), pFile);

    fclose(pFile);

}


//Just for testing
void FileSystem::ps(){
    //Initialize Inode to get a inode
    Inode test;
    readInode(test, 20);
    cout << test.block_count << endl;
    cout << test.indirect_block_address << endl;

    //Update the Inode and put back for testing
    test.block_count = 300;
    updateInode(test, 20);
    
    //Read Inode again
    readInode(test, 20);
    cout << test.block_count << endl;
    cout << test.indirect_block_address << endl;

}


