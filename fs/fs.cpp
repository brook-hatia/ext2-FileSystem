#include <stdio.h>
#include <string.h>
#include <iostream>
#include "fs.h"

using namespace std;

FileSystem::FileSystem(){

}

FileSystem::~FileSystem(){
    
}

// Reads data from disk and store in a structure
void FileSystem::read_disk(auto &x, int blockNum){
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
void FileSystem::write_to_disk(auto x, int len, int blockNum){
  
    // Open File for updating
    FILE *pFile = fopen ("disk", "r+b");

    // Initialize buffer to store data
    char buffer[len];

    // Setting buff as same byte size as node structure and fill with 0
    memset(buffer, 0, len);
    // Copy the data from the struct into the buff
    memcpy(buffer, &x, sizeof(x));

    //get starting of file move file pointer to the start
    fseek(pFile, blockNum * BLOCK_SIZE, SEEK_SET);
    //write the structure into disk
    fwrite(buffer, sizeof(char), sizeof(x), pFile);

    fclose(pFile);

}

// Initializes the File System
void FileSystem::initialize_File_System(){
    
    // Instance Variables
    FILE *pFile;

    // Check if Disk Already exists if not initialize the disk
    if ((pFile = fopen ("disk", "r"))){
        fclose(pFile);
        // Load Inode and Block Bitmap from disk
        read_disk(bm, 0);
        read_disk(im, 1);
    } else {

        pFile = fopen ("disk", "wb");

        // intialize null buffer
        char buffer[BLOCK_SIZE];
        memset(buffer, 0, BLOCK_SIZE);

        //Using buffer to initialize whole disk as NULL
        for (int i = 0; i < TOTAL_BLOCK_NUM; ++i){
            fwrite(buffer, 1, BLOCK_SIZE, pFile);
        }

        fclose(pFile);

        //Initialize block bit map and write it to disk
        for (int i = 0; i < TOTAL_BLOCK_NUM; ++i){
            bm.bmap[i] = '0';
        }
        
        // Mark block used by the bit maps and inode
        for (int i = 0; i< 2+TOTAL_INODE_NUM/32; ++i){
            bm.bmap[i] = '1';
        }
        write_to_disk(bm, sizeof(blockBitmap), 0);

        //Initialize inode bit map and write it to disk
        for (int i = 0; i < TOTAL_INODE_NUM; ++i){
            im.imap[i] = '0';
        }

        // Mark inode bit map for root directory
        im.imap[0] = '1';
        write_to_disk(im, sizeof(iNodeBitmap), 1);

        //Initialize inodes;
        for(int i = 0; i<TOTAL_INODE_NUM; ++i){
            inodeArray[i] = Inode();
            inodeArray[i].block_count = 0;
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

// Update a inode
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

// When File System Terminate stores everything back to disk
void FileSystem::terminate_File_System(){

    write_to_disk(bm, sizeof(blockBitmap), 0);
    write_to_disk(im, sizeof(iNodeBitmap), 1);

}

// Gets a free inode and its number and mark bitmap
int FileSystem::get_free_inode(){

    int rc = -1;

    // Loop through bitmap to find free inode
    for (int i = 0; i < TOTAL_INODE_NUM; i++) {
        if (im.imap[i] == '0') {
            rc = i;
            im.imap[rc] = '1';
            break;
        }
    }

    return rc;
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
    
    cout << im.imap[0] <<endl;
    cout << bm.bmap[0] <<endl;
    im.imap[0] = '1';
    bm.bmap[0] = '0';
    terminate_File_System();

    cout << get_free_inode() << endl;
    cout << "size: " << sizeof(struct directory) << endl;
    cout << "size: " << sizeof(struct directoryEntry) << endl;

}




