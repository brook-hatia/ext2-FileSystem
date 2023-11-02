#include <iostream>
#include <string>
#include "fs.h"

using namespace std;

int main()
{
    FileSystem fs;
    fs.write_to_disk("public");
    fs.read_from_disk("0:0");
}