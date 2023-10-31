#ifndef inode_h
#define inode_h
#include <string>

using namespace std;

class inode
{
    friend class User;

public:
    // meta data
    string Mode;
    int block_count;
    int link_count;
    int uid;
    int gid;
    int file_size;
    std::string creation_time;
    std::string modified_time;
    std::string read_time;
    //

    // next inode elements
    int *block_pointers; // 12 block pointers
    int **single_indirect;
    int ***double_indirect;
    int ****triple_indirect;
    //

    inode();
    ~inode();
};

class inodeList
{

public:
    int *bitmap;
    inodeList();
    ~inodeList();

    int inode_lookup(); // search the next free inode bitmap
};

#endif