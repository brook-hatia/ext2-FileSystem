#ifndef fs_h
#define fs_h
#include <fstream>
#include <string.h>
using namespace std;

// Data
#define BLOCK_SIZE 4096
#define TOTAL_BLOCK_NUM 4096
#define TOTAL_INODE_NUM 1024

class FileSystem
{

public:
    // block
    struct Block
    {
        char text[4096];
    };

    // Size is exactly 128
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

    // size is exactly 256
    struct directoryEntry
    {
        char name[250];
        int inodeNumber;
    };

    // Size is exactly 4096
    struct directory
    {
        struct directoryEntry dirEntries[16];
    };

    // Array of Inode
    struct Inode inodeArray[TOTAL_INODE_NUM];
    struct iNodeBitmap
    {
        char imap[1024];
    };
    struct blockBitmap
    {
        char bmap[4096];
    };

    // Variables
    blockBitmap bm;    // block bit map
    iNodeBitmap im;    // inode bit map
    directory wd;      // working directory
    int initDataBlock; // first block for file data

    // Functions
    FileSystem();
    ~FileSystem();

    bool check_disk(); // check if disk exists

    int get_free_inode(); // search for free inode

    void initialize_inode(Inode &inode, int uid, int linkCount, int fileSize, string mode, int creTime, int modTime, int reTime); // initialize new inode with generic values
    int get_eight_free_block();                                                                                                   // search for 8 free blocks
    int get_free_block();                                                                                                         // search for free block
    void terminate_File_System();                                                                                                 // stores everything back to disk
    void initialize_File_System();                                                                                                // initialize disk with bitmaps, and inodes
    template <typename T>
    void write_to_disk(T x, int len, int blockNum); // write block with text "str" on disk
    template <typename T>
    void read_disk(T &x, int blockNum); // read inode with pointer[0] = str from the list of inodes on the disk
    void readInode(Inode &i, int inodeNum);
    void updateInode(Inode i, int inodeNum);
    int my_mkdir(string directoryName); // returns -1 if file is not created, else return 0;
    void my_cd(string directoryName);
    void my_ls();

    //////////
    string *scan(char *parameter);            // identify function names from filenames/paths
    string identify_function(string *prompt); // call appropriate functions from prompt
    void ps();                                // Just for testing
};

#endif