#include <stdio.h>
#include <string.h>

int main ()
{
    int BLOCK_SIZE = 4096;
    char buffer[BLOCK_SIZE];
    FILE *pFile = fopen ("disk", "wb");
    memset(buffer, 0, BLOCK_SIZE); // intialize null buffer

    /* Using buffer to initialize whole disk as NULL  */
    for (int i = 0; i < 524288; ++i)
        fwrite(buffer, 1, BLOCK_SIZE, pFile);

    fclose(pFile);

   return 0;
}