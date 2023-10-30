#include <iostream>
#include <string>
#include "fs.h"

using namespace std;

int main()
{
    FileSystem *fs;

    // FILE *pFile = fopen("disk", "rb+");
    // if (pFile == NULL) // file system doesn't exist
    // {
    //     inode *in = new inode();

    //     fs = new FileSystem();
    //     fs->write_to_disk("0:0", in); // write "root" directory on first block
    //     fclose(pFile);
    // }
    // inode *in = new inode();
    // fs = new FileSystem();
    // fs->write_to_disk("0:0", in); // write "root" directory on first block

    cout << fs->read_from_disk();

    // FileSystem *fs = new FileSystem(); // allocate memory for FileSystem object

    // inode *in = new inode();

    // fs->write_to_disk("0:0", in); // write "root" directory on the first block
    // cout << fs->read_from_disk();
}