#include "fs.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

int main ()
{
    FileSystem fs;
    fs.initialize_File_System();
    fs.ps();

   return 0;
}
