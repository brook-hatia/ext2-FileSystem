#include <iostream>
#include <string>
#include "fs.h"

using namespace std;

int main()
{
    FileSystem *fs = new FileSystem();

    fs->write_to_disk("users");
    cout << fs->read_from_disk("0:0");

    return 0;
}