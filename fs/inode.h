#ifndef inode_h
#define inode_h
#include <string>
#include <chrono>

using namespace std;

class inode
{
    friend class inodeTable;

public:
    // meta data
    string Mode;
    int inode_num;
    int link_count;
    int uid;
    int gid;
    int file_size;
    time_t creation_time;
    time_t modified_time;
    time_t read_time;
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

class inodeTable
{

public:
    int **bitmap;
    inode **inodes;
    inodeTable(int size);
    ~inodeTable();

    int *inode_lookup();
    void create_inode(inode *new_inode);
};

#endif