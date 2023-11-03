#ifndef fs_h
#define fs_h
#include <fstream>

// Data
#define BLOCK_SIZE 4096
#define TOTAL_BLOCK_NUM 4096
#define TOTAL_INODE_NUM 1024

class FileSystem
{

public:
    struct iNodeBitmap
    {
        char imap[4096];
    };

    struct blockBitmap
    {
        char bmap[4096];
    };

    // block
    struct Block
    {
        char text[4096];
    };

    // inode
    struct Inode
    {
        // meta data
        char Mode[12];
        int block_count;
        int link_count;
        int uid;
        int gid;
        int file_size;
        char creation_time[14];
        char modified_time[14];
        char read_time[14];
        int direct_block_pointers[12];
        int indirect_block_address;
    };

    struct Inode inodeArray[TOTAL_INODE_NUM];

    /************ Brook Hatia ***************/
    struct Directory
    {
        std::string name[146];
        int inode_num[146];
    };

    /************ end Brook Hatia ***************/

    // users
    struct User
    {
        std::string permission;
    };

    /************ Brook Hatia ***************/

    struct Directory *curr_dir;

    /************ end Brook Hatia ***************/

    FileSystem();
    ~FileSystem();

    /************ Brook Hatia ***************/

    bool check_disk(); // check if disk exists

    int inode_lookup(iNodeBitmap &ibm, bool change_bit); // search for free inode

    /************ end Brook Hatia ***************/

    void initialize_inode(Inode &inode); // initialize new inode with generic values

    void initialize_File_System(); // initialize disk with bitmaps, and inodes

    void initialize_dir(Directory &dir); // initialize directory
    template <typename T>
    void write_to_disk(T x, int len, int blockNum); // write block with text "str" on disk
    template <typename T>
    int read_disk(T &x, int blockNum); // read inode with pointer[0] = str from the list of inodes on the disk
    void readInode(Inode &i, int inodeNum);
    void updateInode(Inode &i, int inodeNum);

    /************ Brook Hatia ***************/

    void my_mkdir(std::string str);
    int dir_index(Directory &dir); // returns index to be used to store names and inode_nums

    /************ end Brook Hatia ***************/

    void ps(); // Just for testing
};

#endif