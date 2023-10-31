#ifndef fs_h
#define fs_h

#include "inode.h"
#include <string>

class FileSystem
{
public:
    inode *curr;
    inodeList *inL;

    FileSystem();
    ~FileSystem();
    int bitmap[40960]; // block bitmap

    bool check_disk();
    void write_to_disk(std::string str);
    std::string read_from_disk(std::string str);
};

class Block
{
    friend class FileSystem;

public:
    char text[4096];
    Block();
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