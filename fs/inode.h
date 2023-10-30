#ifndef inode_h
#define inode_h
#include <string>
#include <chrono>

using namespace std;

class inode
{
    friend class inodeTable;
    friend class User;

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

class inodeList
{

public:
    int *bitmap;
    inode **inodes;
    inodeList();
    ~inodeList();

    int free_inode_lookup(); // to use it in fs.cpp
    void create_inode(inode *new_inode);
    inode *Search_inode_from_table(int num);

    // // inode read/write
    // void write_inode_to_disk();
    // void read_inode_from_disk(bool display);

    // // content read/write
    // int write_content_to_disk(string str, int offset);
    // void read_content_from_disk(string str, int offset, int endPos, bool display);
};

#endif