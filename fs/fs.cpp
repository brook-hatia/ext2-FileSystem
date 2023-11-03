#include <stdio.h>
#include <string.h>
#include <iostream>
#include "fs.h"

using namespace std;

FileSystem::FileSystem(){
    
}

FileSystem::~FileSystem(){
    
}

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
    char test_buff[len];

    FILE *pFile = fopen ("disk", "r+b");

    //setting buff as same byte size as node structure and fill with 0
    memset(test_buff, 0, len);
    //copy to test buff from test
    memcpy(test_buff, &x, sizeof(x));

    //get starting of file move file pointer to the start
    fseek(pFile, blockNum * BLOCK_SIZE, SEEK_SET);
    //write the test node structure into start of file
    fwrite(test_buff, sizeof(char), sizeof(x), pFile);

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
        im.imap[i] = '0';
    }
    write_to_disk(im, sizeof(iNodeBitmap), TOTAL_BLOCK_NUM/BLOCK_SIZE);

    //Initialize inodes;
    


}

void FileSystem::ps(){
    cout << sizeof(Inode);
}


