#include <iostream>
#include <string>
#include "fs.h"

using namespace std;

int main()
{
    FileSystem *fs = new FileSystem();

    if (!fs->check_disk())
    {
        fs->write_to_disk("0:0"); // write "root" directory on first block
    }
    else
    {
        fs->write_to_disk("users");
        // fs->write_to_disk("public");
        // fs->write_to_disk("other");
        cout << fs->read_from_disk();
    }

    return 0;
}