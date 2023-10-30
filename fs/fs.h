#ifndef fs_h
#define fs_h

#include "inode.h"
#include <string>

class FileSystem
{
public:
    inode *prev;
    inode *curr;
    inodeList *inL;

    FileSystem();
    ~FileSystem();
    static int bitmap[40960]; // block bitmap, static to use it in fs.cpp
    void write_to_disk(std::string str, inode *in);
    std::string read_from_disk();
};

struct Block
{
    friend class FileSystem;
    char text[4096];
};

class User
{
    friend class FileSystem;

public:
    int permission;
    User();
    ~User();
};

#endif