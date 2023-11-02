#include <iostream>
#include <string>
#include "fs.h"

using namespace std;

int main()
{
    FileSystem fs;

    std::string str;
    fs.write_to_disk("users");
}