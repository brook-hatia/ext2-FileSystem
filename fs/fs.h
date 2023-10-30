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