#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include "type.h"

// ---------------
// | super block |
// ---------------
// |  inode map  |
// ---------------
// |  block map  |
// ---------------
// |    inode    |
// ---------------
// |    data     |
// ---------------

#define KFS_MAGIC (0x66666666)
#define OFFSET_256M (0X10000000)
#define OFFSET_512M (0x20000000)
#define OFFSET_1G (0x40000000)
#define OFFSET_2G (0x80000000)
#define OFFSET_3G (0xC000SAM0000)

#define OFF_512M 0x100000
#define SMALL_FILE 0xB000
#define MIDDLE_FILE 0x40B000
#define BIG_FILE 0x10040B000

//#define FS_SIZE (0x20000000)
#define FS_SIZE (0x20000000) //2^28 = 256MB
#define OFFSET_FS OFFSET_256M
#define NUM_IMAP_SECTOR 1
#define SECTOR_SIZE 512
#define BLOCK_SIZE 0x1000
#define NUM_FD 15

#define ENTRY_SECTOR 8

#define TYPE_FILE 1
#define TYPE_DIR 2
#define TYPE_DOT 3
#define TYPE_DOTDOT 4

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3

#define WRITE 1
#define READ  0
#define INDIRECT_START 129945

//initial addr
#define SUPERBLK_ADDR   0xffffffffa0f00000
#define INODEMAP_ADDR   0xffffffffa0f00200
#define BLKMAP_ADDR     0xffffffffa0f00a00
#define INODE_ADDR      0xffffffffa0f20a00
#define DATABLK_ADDR    0xffffffffa0f64a00

#define DIR_BLOCK_NUM 11
#define MAX_NAME_LEN 10

// offset is sector offset
typedef struct super_block
{
    uint64_t fs_size;
    uint64_t magic;
    //数据块个数
    uint32_t blockmap_bnum;
    uint32_t inodemap_bnum;
    uint32_t inode_bnum;
    uint32_t data_bnum;
    //扇区个数
    uint32_t blockmap_snum;
    uint32_t inodemap_snum;
    uint32_t inode_snum;
    uint32_t data_snum;
    //起始的数据块号
    uint32_t blockmap_boffset;
    uint32_t inodemap_boffset;
    uint32_t inode_boffset;
    uint32_t data_boffset;
    //起始的扇区号
    uint32_t blockmap_soffset;
    uint32_t inodemap_soffset;
    uint32_t inode_soffset;
    uint32_t data_soffset;
    //大小
    uint32_t blockmap_size;
    uint32_t inodemap_size;
    uint32_t inode_size;
    uint32_t data_size;
} super_block_t;

typedef struct inode_entry 
{
    uint32_t inode_id;
    uint32_t mode;//权限 r,w,r/w
    uint32_t file_size;//字节

    //直接 寻址
    uint32_t direct[DIR_BLOCK_NUM];//块号索引
    //间接 寻址
    uint32_t first;
    uint32_t second;
    uint32_t reference;//链接个数
} inode_entry_t;

 
typedef struct dir_entry  
{
    char name[MAX_NAME_LEN];
    uint32_t  type; //0: NONE, 1: file, 2: dentry, 3:".", 4:".."
    uint32_t  inode_id;  
} dir_entry_t;

typedef struct fd
{//文件描述符
    uint32_t inode_id;
    uint8_t mode;
    uint32_t read_pos;
    uint32_t write_pos; 
} fd_t;

// extern inode_entry_t current_dir_entry;

void init_fs();
void print_fs();
 
void read_super_block();

 
int readdir(char *name);
int mkdir(char *name);
int rmdir(char *name);
int mknod(char *name);

int open(char *name, uint32_t access);
int write(uint32_t fd, char *buff, uint32_t size);
int read(uint32_t fd, char *buff, uint32_t size);
int close(uint32_t fd);
int do_cat(char *name);

int do_statfs(void);
int do_mkfs(void);
int do_ls(char *);
int do_cd(char *);
int do_touch(char *);
void check_fs(void);
void init_fds(void);
int do_ln(char *, char *);
int do_ln_s(char *, char *);
int do_ll(char *);
int fsearch(uint32_t, int, uint32_t);

#endif