#include <stdio.h>
#include <string.h>
#include <iostream>
#define BLOCK_SIZE 4096
using namespace std;

template <typename T>
int read_disk(int blockNum, T &x){
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

template <typename T>
void write_to_disk(T x, int len, int blockNum){
    //get size of struct
    
    //set buffer
    char test_buff[len];

    FILE *pFile = fopen ("disk", "wb");

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

struct node{
    char text[64];
    int i;
};

struct inodeBitmap{
    char text[2];
};

int main ()
{
    //Set Block size

    char buffer[BLOCK_SIZE];
    FILE *pFile = fopen ("disk", "wb");
    memset(buffer, 0, BLOCK_SIZE); // initialize null buffer

    /* Using buffer to initialize whole disk as NULL  */
    for (int i = 0; i < 2; ++i)
        fwrite(buffer, 1, BLOCK_SIZE, pFile);

    fclose(pFile);

    inodeBitmap y;
    
    y.text[0] = '2';
    y.text[1] = '3';

    for(int i =0; i<2; i++){
        cout << y.text[i];
    }
    

    int len = sizeof(inodeBitmap);
    write_to_disk(y,len,0);

    //initialize test structure
    //node test = {.text = "Works!", .i=100};
    //int len = sizeof(node);
    //write_to_disk(test, len);

    inodeBitmap x;
    read_disk(0,x);
    
    for(int i =0; i<2; i++){
        cout << x.text[i];
    }
    
    // Now you can access the data in the 'read_test' structure
    //cout << x.i << x.text;

   return 0;
}