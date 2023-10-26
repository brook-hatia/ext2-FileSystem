#include <stdio.h>
#include <string.h>

struct node{
    char text[64];
};

int main ()
{
    //Set Block size
    int BLOCK_SIZE = 4096;
    char buffer[BLOCK_SIZE];
    FILE *pFile = fopen ("disk", "wb");
    memset(buffer, 0, BLOCK_SIZE); // intialize null buffer

    /* Using buffer to initialize whole disk as NULL  */
    for (int i = 0; i < 2; ++i)
        fwrite(buffer, 1, BLOCK_SIZE, pFile);

    //initialize test structure
    node test = {.text = "Hello World"};
    //get size of struct
    int len = sizeof(struct node);
    //set buffer
    char test_buff[len];

    //setting buff as same byte size as node structure and fill with 0
    memset(test_buff, 0, len);
    //copy to test buff from test
    memcpy(test_buff, &test, sizeof(test));

    //get starting of file move file pointer to the start
    fseek(pFile, 0, SEEK_SET);
    //write the test node structure into start of file
    fwrite(test_buff, sizeof(char), sizeof(test), pFile);

    fclose(pFile);

    // Reopen the file for reading
    pFile = fopen("disk", "rb");

    if (pFile == NULL) {
        perror("Error opening the file for reading");
        return 1;
    }

    //get starting of file move file pointer to the start
    fseek(pFile, 0, SEEK_SET);
    
    //initialize buffer to store data
    char read_buffer[BLOCK_SIZE];
    // Read the data from the file into a buffer
    fread(read_buffer, 1, BLOCK_SIZE, pFile);

    // Close the file after reading
    fclose(pFile);

    // Initialize a new struct to store the data
    struct node read_test;

    // Copy the data from the buffer into the struct
    memcpy(&read_test, read_buffer, sizeof(read_test));

    // Now you can access the data in the 'read_test' structure
    printf("Text: %s\n", read_test.text);



   return 0;
}