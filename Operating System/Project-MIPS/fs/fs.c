#include "fs.h"
#include "string.h"
#include "common.h"
#include "sched.h"

static uint8_t zero[SECTOR_SIZE]; // always zero
static uint8_t sup[SECTOR_SIZE];  // super block buffer
static super_block_t *super_block = (super_block_t *)SUPERBLK_ADDR;
static char *inode_map = (char *)INODEMAP_ADDR;
static char *block_map = (char *)BLKMAP_ADDR;
inode_entry_t *inode_entry = (inode_entry_t *)INODE_ADDR; //初始化根目录

static fd_t fds[NUM_FD];
static inode_entry_t *current_dir_entry = (inode_entry_t *)INODE_ADDR; //初始化成根目录
int block_id = 0;

int readdir(char *name)
{
    return 0;
}

int mknod(char *name)
{

    return 0; // failure
}

int alloc_datablock()
{
    int i;
    for (i = 0; i < INODE_ADDR - BLKMAP_ADDR; i++)
    {
        if (block_map[i] == 0)
            return i;
    }
    return -1;
}

int alloc_inode()
{
    int i;
    for (i = 0; i < DATABLK_ADDR - INODE_ADDR; i++)
    {
        if (inode_map[i] == 0)
            return i;
    }
    return -1;
}

void init_dentry(dir_entry_t *entry, int curnode_id, int parnode_id)
{
    strcpy((char *)entry[0].name, ".");
    entry[0].type = TYPE_DOT;
    entry[0].inode_id = curnode_id;
    strcpy((char *)entry[1].name, "..");
    entry[1].type = TYPE_DOTDOT;
    entry[1].inode_id = parnode_id;
}

int init_inode(uint32_t f_sz) //初始化inode
{
    uint32_t index;
    index = alloc_inode();
    inode_entry[index].inode_id = index;
    inode_map[index] = 1;
    if (f_sz == 0) //目录的 inode
    {
        int block_num;
        block_num = alloc_datablock();
        inode_entry[index].direct[0] = block_num;
        inode_entry[index].mode = O_RDWR;
        block_map[block_num] = 1;
    }
    return index;
}

sd_wr_block(int block_num, int worr) //worr 为1 表示写，为0 表示读
{
    //unsigned char * x = (unsigned char *)DATABLK_ADDR + block_num * BLOCK_SIZE;
    //unsigned int b = (unsigned int) OFFSET_512M + super_block->data_soffset * SECTOR_SIZE + block_num * BLOCK_SIZE;
    if (worr) //写
        sdwrite((unsigned char *)(DATABLK_ADDR + block_num * BLOCK_SIZE), (unsigned int)(OFFSET_512M + super_block->data_soffset * SECTOR_SIZE + block_num * BLOCK_SIZE), BLOCK_SIZE);
    else //读
        sdread((unsigned char *)(DATABLK_ADDR + block_num * BLOCK_SIZE), (unsigned int)(OFFSET_512M + super_block->data_soffset * SECTOR_SIZE + block_num * BLOCK_SIZE), BLOCK_SIZE);
}

sd_wr_blockmap(int worr) //1写，0读
{
    if (worr)
        sdwrite((unsigned char *)BLKMAP_ADDR, (unsigned int)(OFFSET_512M + super_block->blockmap_soffset * SECTOR_SIZE), super_block->blockmap_size);
    else
        sdread((unsigned char *)BLKMAP_ADDR, (unsigned int)(OFFSET_512M + super_block->blockmap_soffset * SECTOR_SIZE), super_block->blockmap_size);
}

sd_wr_inodemap(int worr) //1写 0读
{
    if (worr)
        sdwrite((unsigned char *)INODEMAP_ADDR, (unsigned int)(OFFSET_512M + super_block->inodemap_soffset * SECTOR_SIZE), super_block->inodemap_size);
    else
        sdread((unsigned char *)INODEMAP_ADDR, (unsigned int)(OFFSET_512M + super_block->inodemap_soffset * SECTOR_SIZE), super_block->inodemap_size);
}

sd_wr_inode(int worr)
{
    if (worr)
        sdwrite((unsigned char *)INODE_ADDR, (unsigned int)(OFFSET_512M + super_block->inode_soffset * SECTOR_SIZE), super_block->inode_size);
    else
        sdread((unsigned char *)INODE_ADDR, (unsigned int)(OFFSET_512M + super_block->inode_soffset * SECTOR_SIZE), super_block->inode_size);
}

sd_wr_superblock(int worr) //后两个都是sector为单位的
{
    if (worr)
        sdwrite((unsigned char *)SUPERBLK_ADDR, (unsigned int)OFFSET_512M, SECTOR_SIZE);
    else
        sdread((unsigned char *)SUPERBLK_ADDR, (unsigned int)OFFSET_512M, SECTOR_SIZE);
}

int do_mkfs()
{
    /*sd_wr_superblock(READ);
    if (super_block->magic == KFS_MAGIC) //已经存在了
    {
        prints("File System already exist!\n");
        return 0;
    }*/

    prints("[FS] Setting superblock...\n");
    //初始化superblock
    super_block->magic = KFS_MAGIC;
    super_block->fs_size = FS_SIZE;
    //inode_map
    super_block->inodemap_soffset = (INODEMAP_ADDR - SUPERBLK_ADDR) / SECTOR_SIZE;
    super_block->inodemap_boffset = (INODEMAP_ADDR - SUPERBLK_ADDR) / BLOCK_SIZE;
    super_block->inodemap_snum = (BLKMAP_ADDR - INODEMAP_ADDR) / SECTOR_SIZE + ((BLKMAP_ADDR - INODEMAP_ADDR) % SECTOR_SIZE != 0);
    super_block->inodemap_bnum = (BLKMAP_ADDR - INODEMAP_ADDR) / BLOCK_SIZE + ((BLKMAP_ADDR - INODEMAP_ADDR) % BLOCK_SIZE != 0);
    super_block->inodemap_size = BLKMAP_ADDR - INODEMAP_ADDR;
    //block_map
    super_block->blockmap_soffset = (BLKMAP_ADDR - SUPERBLK_ADDR) / SECTOR_SIZE;
    super_block->blockmap_boffset = (BLKMAP_ADDR - SUPERBLK_ADDR) / BLOCK_SIZE;
    super_block->blockmap_snum = (INODE_ADDR - BLKMAP_ADDR) / SECTOR_SIZE + ((INODE_ADDR - BLKMAP_ADDR) % SECTOR_SIZE != 0);
    super_block->blockmap_bnum = (INODE_ADDR - BLKMAP_ADDR) / BLOCK_SIZE + ((INODE_ADDR - BLKMAP_ADDR) % BLOCK_SIZE != 0);
    super_block->blockmap_size = INODE_ADDR - BLKMAP_ADDR;
    //inode
    super_block->inode_soffset = (INODE_ADDR - SUPERBLK_ADDR) / SECTOR_SIZE;
    super_block->inode_boffset = (INODE_ADDR - SUPERBLK_ADDR) / BLOCK_SIZE;
    super_block->inode_snum = (DATABLK_ADDR - INODE_ADDR) / SECTOR_SIZE + ((DATABLK_ADDR - INODE_ADDR) % SECTOR_SIZE != 0);
    super_block->inode_bnum = (DATABLK_ADDR - INODE_ADDR) / BLOCK_SIZE + ((DATABLK_ADDR - INODE_ADDR) % BLOCK_SIZE != 0);
    super_block->inode_size = DATABLK_ADDR - INODE_ADDR;
    //datablock
    super_block->data_soffset = (DATABLK_ADDR - SUPERBLK_ADDR) / SECTOR_SIZE;
    super_block->data_boffset = (DATABLK_ADDR - SUPERBLK_ADDR) / BLOCK_SIZE;
    super_block->data_snum = (FS_SIZE - (DATABLK_ADDR - SUPERBLK_ADDR)) / SECTOR_SIZE + ((FS_SIZE - (DATABLK_ADDR - SUPERBLK_ADDR)) % SECTOR_SIZE != 0);
    super_block->data_bnum = (FS_SIZE - (DATABLK_ADDR - SUPERBLK_ADDR)) / BLOCK_SIZE + ((FS_SIZE - (DATABLK_ADDR - SUPERBLK_ADDR)) % BLOCK_SIZE != 0);
    super_block->data_size = FS_SIZE - (DATABLK_ADDR - SUPERBLK_ADDR);
    //打印
    prints("     magic : %x\n", super_block->magic);
    prints("     num sector : %d, num block : %d\n", super_block->fs_size / SECTOR_SIZE, super_block->fs_size / BLOCK_SIZE);
    prints("     inode map offset : %d(%d) sector, %d(%d) block\n", super_block->inodemap_soffset, super_block->inodemap_snum, super_block->inodemap_boffset, super_block->inodemap_bnum);
    prints("     block map offset : %d(%d) sector, %d(%d) block\n", super_block->blockmap_soffset, super_block->blockmap_snum, super_block->blockmap_boffset, super_block->blockmap_bnum);
    prints("     inode offset : %d(%d) sector, %d(%d) block\n", super_block->inode_soffset, super_block->inode_snum, super_block->inode_boffset, super_block->inode_bnum);
    prints("     data offset : %d(%d) sector, %d(%d) block\n", super_block->data_soffset, super_block->data_snum, super_block->data_boffset, super_block->data_bnum);
    prints("     inode entry size : %dB\n", sizeof(inode_entry_t));
    prints("     dir entry size : %dB\n", sizeof(dir_entry_t));
    sd_wr_superblock(WRITE);

    //初始化inode_map
    prints("[FS] Setting inode-map...\n");
    bzero((void *)INODEMAP_ADDR, BLKMAP_ADDR - INODEMAP_ADDR);

    //初始化block_map
    prints("[FS] Setting block-map...\n");
    bzero((void *)BLKMAP_ADDR, INODE_ADDR - BLKMAP_ADDR);

    //初始化inode
    prints("[FS] Setting inode...\n");
    bzero((void *)INODE_ADDR, DATABLK_ADDR - INODE_ADDR);
    uint32_t inode_index;
    inode_index = init_inode(0);
    inode_entry[inode_index].file_size = 2;
    current_dir_entry = &inode_entry[inode_index];
    //初始化dentry
    init_dentry((dir_entry_t *)(DATABLK_ADDR + inode_entry[0].direct[0] * BLOCK_SIZE), current_dir_entry->inode_id, current_dir_entry->inode_id);
    current_dir_entry->file_size = 2; //表示两个目录项

    sd_wr_block(inode_entry[0].direct[0], WRITE);
    sd_wr_inodemap(WRITE);
    sd_wr_blockmap(WRITE);
    sd_wr_inode(WRITE);

    prints("[FS] Initialize filesystem finished!\n");
    return 1;
}

int do_statfs()
{
    //读取superblock 打印信息
    sd_wr_superblock(READ);
    sd_wr_inode(READ);
    sd_wr_inodemap(READ);
    sd_wr_blockmap(READ);

    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return 0;
    }

    prints("magic : %x (KFS)\n", super_block->magic);
    int block_map_count, i;
    block_map_count = 0;
    //一般信息
    prints("inode map : %d(%d) sector, %d(%d) block\n", super_block->inodemap_soffset, super_block->inodemap_snum, super_block->inodemap_boffset, super_block->inodemap_bnum);
    prints("block map : %d(%d) sector, %d(%d) block\n", super_block->blockmap_soffset, super_block->blockmap_snum, super_block->blockmap_boffset, super_block->blockmap_bnum);
    prints("inode     : %d(%d) sector, %d(%d) block\n", super_block->inode_soffset, super_block->inode_snum, super_block->inode_boffset, super_block->inode_bnum);
    prints("data      : %d(%d) sector, %d(%d) block\n", super_block->data_soffset, super_block->data_snum, super_block->data_boffset, super_block->data_bnum);
    prints("inode entry size : %dB, dir entry size : %dB\n", sizeof(inode_entry_t), sizeof(dir_entry_t));
    for (i = 0; i < 0x20000; i++)
    {
        if (block_map[i])
            block_map_count++;
    }
    prints("used block : %d/%d\n", block_map_count, super_block->data_bnum);
    int inode_map_count;
    inode_map_count = 0;
    for (i = 0; i < 0x800; i++)
    {
        if (inode_map[i])
            inode_map_count++;
    }
    prints("used inode : %d/%d \n", inode_map_count, 0x800);
    return 1;
}

int mkdir(char *name)
{
    sd_wr_superblock(READ);
    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return 0;
    }
    sd_wr_inode(READ);
    sd_wr_inodemap(READ);
    sd_wr_blockmap(READ);

    //查找父目录是否存在
    int i;
    dir_entry_t *entry_start;
    entry_start = (dir_entry_t *)(DATABLK_ADDR + current_dir_entry->direct[0] * BLOCK_SIZE);
    // 注意 读当前inode指向的block
    sd_wr_block(current_dir_entry->direct[0], READ);
    while (entry_start->name[0] != '\0')
    {
        if (entry_start->type == TYPE_DIR && strcmp(entry_start->name, name) == 0) //已经存在
        {
            prints("Dir already exist!\n");
            return 0;
        }
        entry_start += sizeof(dir_entry_t);
    }
    //步骤1：新建一个指向dir的inode，初始化该inode和目录项
    int inode_index;
    inode_index = init_inode(0);
    dir_entry_t *a1 = (dir_entry_t *)(DATABLK_ADDR + inode_entry[inode_index].direct[0] * BLOCK_SIZE);
    init_dentry((dir_entry_t *)(DATABLK_ADDR + inode_entry[inode_index].direct[0] * BLOCK_SIZE), inode_entry[inode_index].inode_id, current_dir_entry->inode_id);
    dir_entry_t *a2 = (dir_entry_t *)(DATABLK_ADDR + inode_entry[inode_index].direct[0] * BLOCK_SIZE);
    inode_entry[inode_index].file_size = 2; //初始化之后置上
    inode_entry[inode_index].reference = 2;
    //步骤2：把刚新建的inode_id和名字 填到父目录项中，
    entry_start = (dir_entry_t *)(DATABLK_ADDR + current_dir_entry->direct[0] * BLOCK_SIZE);
    strcpy(entry_start[current_dir_entry->file_size].name, name);
    entry_start[current_dir_entry->file_size].type = TYPE_DIR;
    entry_start[current_dir_entry->file_size].inode_id = inode_entry[inode_index].inode_id;
    current_dir_entry->file_size++;
    //写回sd卡
    //新建的inode指向的block和修改的父目录block
    sd_wr_block(inode_entry[inode_index].direct[0], WRITE); //bug
    sd_wr_block(current_dir_entry->direct[0], WRITE);
    sd_wr_inode(WRITE);
    sd_wr_inodemap(WRITE);
    sd_wr_blockmap(WRITE);
    return 1;
}

int rmdir(char *name)
{
    //删除
    sd_wr_superblock(READ);
    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return 0;
    }
    sd_wr_inode(READ);
    sd_wr_inodemap(READ);
    sd_wr_blockmap(READ);
    int i, j;
    dir_entry_t *start;
    start = (dir_entry_t *)(DATABLK_ADDR + current_dir_entry->direct[0] * BLOCK_SIZE);
    //读当前目录的block
    sd_wr_block(current_dir_entry->direct[0], READ);
    for (i = 0; i < current_dir_entry->file_size; i++)
    {
        if (strcmp(start[i].name, name) == 0) //名字对上
        {
            //目录direct索引到的块
            block_map[inode_entry[start[i].inode_id].direct[0]] = 0;
            bzero((void *)(DATABLK_ADDR + inode_entry[start[i].inode_id].direct[0] * BLOCK_SIZE), BLOCK_SIZE); //直接清零写进去，不用读
            sd_wr_block(inode_entry[start[i].inode_id].direct[0], WRITE);
            //inode
            inode_map[start[i].inode_id] = 0;
            bzero(&inode_entry[start[i].inode_id], sizeof(inode_entry_t));
            //block，需要将后面的向前移动
            for (j = i + 1; j < current_dir_entry->file_size; j++, i++)
            {
                memcpy((void *)&start[i], (void *)&start[j], sizeof(dir_entry_t));
            }
            sd_wr_block(current_dir_entry->direct[0], WRITE);
            //file_size
            current_dir_entry->file_size--;
            break;
        }
    }
    sd_wr_inode(WRITE);
    sd_wr_inodemap(WRITE);
    sd_wr_blockmap(WRITE);
    //.和..不符手动删除，所以不会出现删之后file_size变成0的情况

    return 0;
}

int get_split_dir(char *name, char dir[3][MAX_NAME_LEN], int *m) //第一个有斜线是绝对路径
{
    int i, j, k;
    i = 0;
    if (name[i] == '/') //绝对路径
    {
        *m = 1;
        i++;
    }
    else //相对路径
        *m = 0;
    //先清零
    for (j = 0; j < 3; j++)
    {
        for (k = 0; k < MAX_NAME_LEN; k++)
            dir[j][k] = 0;
    }
    for (j = 0; j < 3; j++)
    {
        for (k = 0; name[i] != '/' && name[i] != '\0' && name[i] != '\r' && name[i] != '\n'; i++, k++)
        {
            dir[j][k] = name[i];
        }
        if (name[i] == '\0' || name[i] == '\r' || name[i] == '\n')
        {
            dir[j][k] = '\0';
            break;
        }
        if (name[i] == '/')
        {
            dir[j][k] = '\0';
            i++;
        }
    }
}

dir_entry_t *find_path(char dir[3][MAX_NAME_LEN], inode_entry_t **trace, int mode1)
{
    dir_entry_t *dentry_trace;
    int i, j, inode_num;

    if (mode1 == 1)               //绝对
        *trace = &inode_entry[0]; //根目录的inode
    else if (mode1 == 0)          //相对
        *trace = current_dir_entry;

    for (i = 0; i < 3 && dir[i][0] != '\0'; i++)
    {
        sd_wr_block((*trace)->direct[0], READ);
        dentry_trace = (dir_entry_t *)(DATABLK_ADDR + (*trace)->direct[0] * BLOCK_SIZE);
        for (j = 0; j < (*trace)->file_size; j++)
        {
            if (strcmp(dentry_trace[j].name, dir[i]) == 0)
                break;
        }
        *trace = &inode_entry[dentry_trace[j].inode_id];
    }
    sd_wr_block((*trace)->direct[0], READ);
    dentry_trace = (dir_entry_t *)(DATABLK_ADDR + (*trace)->direct[0] * BLOCK_SIZE);
    return dentry_trace;
}

int do_ls(char *next)
{
    int i, mode;
    char split_dir[3][MAX_NAME_LEN];
    inode_entry_t *track;
    sd_wr_superblock(READ);
    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return 0;
    }
    sd_wr_inode(READ);
    sd_wr_block(current_dir_entry->direct[0], READ);
    dir_entry_t *start;
    if (*next == 0)
    {
        start = (dir_entry_t *)(DATABLK_ADDR + current_dir_entry->direct[0] * BLOCK_SIZE);
        track = current_dir_entry;
    }
    else
    {
        get_split_dir(next, split_dir, &mode);      //mode = 1 绝对 mode = 0 相对
        start = find_path(split_dir, &track, mode); //传指针,注意需要读block
    }
    for (i = 0; i < track->file_size; i++)
        prints("%s  ", start[i].name);
    prints("\n");
    return 1;
}

int do_ll(char *next)
{
    int i, mode;
    char split_dir[3][MAX_NAME_LEN];
    inode_entry_t *track;
    sd_wr_superblock(READ);
    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return 0;
    }
    sd_wr_inode(READ);
    sd_wr_block(current_dir_entry->direct[0], READ);
    dir_entry_t *start;
    if (*next == 0)
    {
        start = (dir_entry_t *)(DATABLK_ADDR + current_dir_entry->direct[0] * BLOCK_SIZE);
        track = current_dir_entry;
    }
    else
    {
        get_split_dir(next, split_dir, &mode);      //mode = 1 绝对 mode = 0 相对
        start = find_path(split_dir, &track, mode); //传指针,注意需要读block
    }
    for (i = 0; i < track->file_size; i++)
    {
        if(start[i].type== TYPE_FILE)
            prints("%s  %d\n", start[i].name, inode_entry[start[i].inode_id].reference);
        else
            prints("%s  %d\n", start[i].name, inode_entry[start[i].inode_id].file_size);
    }
    return 1;
}

int do_cd(char *next)
{
    int i, mode;
    char split_dir[3][MAX_NAME_LEN];
    inode_entry_t *track;
    sd_wr_superblock(READ);
    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return 0;
    }
    sd_wr_inode(READ);
    sd_wr_block(current_dir_entry->direct[0], READ);
    dir_entry_t *start;
    get_split_dir(next, split_dir, &mode); //mode = 1 绝对 mode = 0 相对
    start = find_path(split_dir, &track, mode);
    current_dir_entry = track;
    return 1;
}

int do_touch(char *name) //建立一个空文件
{
    sd_wr_superblock(READ);
    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return 0;
    }
    sd_wr_inode(READ);
    sd_wr_inodemap(READ);
    sd_wr_blockmap(READ);
    int inode_index;
    inode_index = init_inode(0);
    inode_entry[inode_index].file_size = 0;
    inode_entry[inode_index].reference = 1;
    //在当前目录中填写
    dir_entry_t *start;
    sd_wr_block(current_dir_entry->direct[0], READ);
    start = (dir_entry_t *)(DATABLK_ADDR + current_dir_entry->direct[0] * BLOCK_SIZE);
    strcpy(start[current_dir_entry->file_size].name, name);
    start[current_dir_entry->file_size].type = TYPE_FILE;
    start[current_dir_entry->file_size].inode_id = inode_index;
    current_dir_entry->file_size++;
    sd_wr_block(current_dir_entry->direct[0], WRITE);
    sd_wr_inode(WRITE);
    sd_wr_inodemap(WRITE);
    sd_wr_blockmap(WRITE);

    return 1;
}

void init_fds()
{
    int i;
    for (i = 0; i < NUM_FD; i++)
        fds[i].inode_id = 0;
}

int open(char *name, uint32_t access) //access是打开权限，返回文件描述符下标
{
    //从文件系统查找文件
    sd_wr_superblock(READ);
    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return 0;
    }
    sd_wr_inode(READ);
    dir_entry_t *start;
    char *data_start;
    int inode_index;
    sd_wr_block(current_dir_entry->direct[0], READ);
    start = (dir_entry_t *)(DATABLK_ADDR + current_dir_entry->direct[0] * BLOCK_SIZE);
    int i;
    for (i = 0; i < current_dir_entry->file_size; i++)
    {
        if (strcmp(start[i].name, name) == 0) //匹配上
        {
            inode_index = start[i].inode_id;
            //分配文件描述符
            int j;
            for (j = 0; j < NUM_FD; j++)
            {
                if (fds[j].inode_id == 0) //空闲,所以要把fds清空
                {
                    fds[j].inode_id = inode_index;
                    fds[j].mode = access;
                    fds[j].read_pos = 0;//debug
                    fds[j].write_pos = 0;
                    current_running[0]->user_context.regs[2] = j;
                    return j;
                }
            }
        }
    }
    current_running[0]->user_context.regs[2] = -1;
    return -1; // open failure
}

int close(uint32_t fd)
{
    //释放文件描述符
    fds[fd].inode_id = 0;
    fds[fd].mode = 0;
    fds[fd].read_pos = 0;
    fds[fd].write_pos = 0;
    return 0;
}
void read_block(int block_index, int offset, int fdt, int *sizet, int *countt, char *buf)
{
    sd_wr_block(block_index, READ);
    char *data_start = (char *)(DATABLK_ADDR + block_index * BLOCK_SIZE + offset);
    int k = 0;
    while (*sizet > 0 && k + offset < BLOCK_SIZE)
    {
        (*buf++) = (*data_start++);
        (*sizet)--;
        k++;
        fds[fdt].read_pos++;
        (*countt)++;
    }
}

int read(uint32_t fd, char *buff, uint32_t size)
{                                 //buff是缓冲区地址，size是读文件的大小
    if (fds[fd].mode == O_WRONLY) //只能写
    {
        prints("error, WRITE only file can't be read\n");
        return -1;
    }
    //read superblock
    sd_wr_superblock(READ);
    prints("flag1");
    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return -1;
    }
    sd_wr_inode(READ);
    prints("flag2");
    int inode_num = fds[fd].inode_id;

    int b_ind, count, off;
    count = 0;

    while (fds[fd].read_pos < SMALL_FILE && size > 0) //直接指针
    {
        b_ind = fds[fd].read_pos / BLOCK_SIZE;
        off = fds[fd].read_pos % BLOCK_SIZE; //起始位置的偏移
        read_block(inode_entry[inode_num].direct[b_ind], off, fd, &size, &count, buff);
    }
    while (fds[fd].read_pos < MIDDLE_FILE && size > 0) //一级间址
    {
        b_ind = fds[fd].read_pos / BLOCK_SIZE - DIR_BLOCK_NUM;
        off = fds[fd].read_pos % BLOCK_SIZE; //起始位置的偏移
        uint32_t *first_base = (uint32_t *)(DATABLK_ADDR + inode_entry[inode_num].first * BLOCK_SIZE);
        //读该block
        sd_wr_block(inode_entry[inode_num].first, READ);
        read_block(first_base[b_ind], off, fd, &size, &count, buff);
    }
    while (fds[fd].read_pos < BIG_FILE && size > 0) //二级间址
    {
        b_ind = fds[fd].read_pos / BLOCK_SIZE - DIR_BLOCK_NUM - 1024;
        off = fds[fd].read_pos % BLOCK_SIZE; //起始位置的偏移
        //二级的一级块
        uint32_t *first_base = (uint32_t *)(DATABLK_ADDR + inode_entry[inode_num].second * BLOCK_SIZE);
        sd_wr_block(inode_entry[inode_num].second, READ);
        //根据b_ind寻址
        int first_ind = b_ind / 1024;
        int second_ind = b_ind % 1024;
        //二级的二级块
        uint32_t *second_base = (uint32_t *)(DATABLK_ADDR + first_base[first_ind] * BLOCK_SIZE);
        sd_wr_block(first_base[first_ind], READ);
        read_block(second_base[second_ind], off, fd, &size, &count, buff);
    }

    //返回实际写入数据大小
    current_running[0]->user_context.regs[2] = count;
    return count;
}

void write_block(int block_index, int offset, int fdt, int *sizet, int *countt, char *buf, int ino) //需要分配新的block
{
    char *data_start = (char *)(DATABLK_ADDR + block_index * BLOCK_SIZE + offset);
    int k = 0; //块内count
    while (*sizet > 0 && k + offset < BLOCK_SIZE)
    {
        (*data_start++) = (*buf++);
        (*sizet)--;
        k++;
        fds[fdt].write_pos++;
        inode_entry[ino].file_size++;
        (*countt)++;
    }
    sd_wr_block(block_index, WRITE);
}

int write(uint32_t fd, char *buff, uint32_t size)
{
    if (fds[fd].mode == O_RDONLY) //只能写读
    {
        prints("error, READ only file can't be written\n");
        return -1;
    }
    //read superblock
    sd_wr_superblock(READ);

    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return -1;
    }
    sd_wr_inode(READ);
    sd_wr_blockmap(READ);

    int inode_num = fds[fd].inode_id;
    //写文件
    int k, count, first_block;
    count = 0;
    while (fds[fd].write_pos < SMALL_FILE && size > 0) //直接
    {
        int block_num = fds[fd].write_pos / BLOCK_SIZE;
        int off = fds[fd].write_pos % BLOCK_SIZE;
        int block_ind;
        //创建block和映射
        if (block_num != 0)
        {
            block_ind = alloc_datablock();
            block_map[block_ind] = 1;
            inode_entry[inode_num].direct[block_num] = block_ind;
        }
        else
            block_ind = inode_entry[inode_num].direct[0];
        //写block
        write_block(block_ind, off, fd, &size, &count, buff, inode_num);
    }
    int flag1 = 0;
    while (fds[fd].write_pos < MIDDLE_FILE && size > 0) //一级间址
    {
        flag1 = 1;
        int block_num = fds[fd].write_pos / BLOCK_SIZE - DIR_BLOCK_NUM;
        int off = fds[fd].write_pos % BLOCK_SIZE;
        int block_ind;
        uint32_t * first_start;
        //分配一级间址块
        if (block_num == 0 && off == 0) //第一次进入
        {
            first_block = alloc_datablock();
            block_map[first_block] = 1;
            first_start = (uint32_t *)(DATABLK_ADDR + first_block * BLOCK_SIZE);
            inode_entry[inode_num].first = first_block;
        }
        else
        {
            first_start = (uint32_t *)(DATABLK_ADDR + inode_entry[inode_num].first * BLOCK_SIZE);
            first_block = inode_entry[inode_num].first;
        }
        //分配块
        if (off == 0) //不像direct已经分配了direct[0]，这次全都要
        {
            block_ind = alloc_datablock();
            block_map[block_ind] = 1;
            first_start[block_num] = block_ind;
        }
        else
            block_ind = first_start[block_num];
        write_block(block_ind, off, fd, &size, &count, buff, inode_num);
    }
    //写间址块
    if (flag1 == 1)
        sd_wr_block(first_block, WRITE);

    int flag2 = 0;
    uint32_t *first_start;
    while (fds[fd].write_pos < BIG_FILE && size > 0) //二级间址
    {
        flag2 = 1;
        int block_num = fds[fd].write_pos / BLOCK_SIZE - DIR_BLOCK_NUM - 1024;
        int off = fds[fd].write_pos % BLOCK_SIZE;
        int find_first = block_num / 1024;
        int find_second = block_num % 1024;
        int block_ind;
        int second_block;
        uint32_t *second_start;
        //分配二级间址的一级间址块
        if (block_num == 0 && off == 0) //第一次进入
        {
            first_block = alloc_datablock();
            block_map[first_block] = 1;
            first_start = (uint32_t *)(DATABLK_ADDR + first_block * BLOCK_SIZE);
            inode_entry[inode_num].second = first_block;
        }
        else
        {
            first_block = inode_entry[inode_num].second;
            first_start = (uint32_t *)(DATABLK_ADDR + first_block * BLOCK_SIZE);
        }
        //分配二级间址的二级间址块
        if (find_second == 0 && off == 0) //第一次进入二级间址块
        {
            second_block = alloc_datablock();
            block_map[second_block] = 1;
            first_start[find_first] = second_block; //写一级
            second_start = (uint32_t *)(DATABLK_ADDR + second_block * BLOCK_SIZE);
        }
        else
        {
            second_start = (uint32_t *)(DATABLK_ADDR + first_start[find_first] * BLOCK_SIZE);
            second_block = first_start[find_first];
        }
        //分配二级间址指向的数据块
        if (off == 0)
        {
            block_ind = alloc_datablock();
            block_map[block_ind] = 1;
            second_start[find_second] = block_ind;
        }
        else
            block_ind = second_start[find_second];
        //写
        write_block(block_ind, off, fd, &size, &count, buff, inode_num);
    }
    //写间址块
    int i;
    if (flag2 == 1)
    {
        sd_wr_block(first_block, WRITE);
        for (i = 0; first_start[i] != 0 && i < 1024; i++)
            sd_wr_block(first_start[i], WRITE);
    }

    sd_wr_inode(WRITE);
    sd_wr_blockmap(WRITE);
    //返回实际写入数据大小
    current_running[0]->user_context.regs[2] = count;
    return count;
}

int do_cat(char *name) //打印一个文件到shell
{
    int fd_ind, num, i;
    fd_ind = open(name, O_RDONLY); //打开
    char *buffer;
    buffer = (char *)(DATABLK_ADDR + inode_entry[fds[fd_ind].inode_id].direct[0] * BLOCK_SIZE);
    num = read(fd_ind, buffer, inode_entry[fds[fd_ind].inode_id].file_size);
    for (i = 0; i < num; i++)
        prints("%c", buffer[i]);
    close(fd_ind);
}

void split_dir_name(char *name, char *dir, char *dirname)
{
    int i, j, ind1, ind2;
    i = strlen(dirname);
    for (; i >= 0; i--)
    {
        if (dirname[i] == '/') //找到第一个/的索引， 他后面就是文件名
            break;
    }
    for (j = 0, ind1 = 0, ind2 = 0; dirname[j] != '\0' && j < strlen(dirname); j++)
    {
        if (j < i)
        {
            dir[ind1] = dirname[j];
            ind1++;
        }
        else if (j > i)
        {
            name[ind2] = dirname[j];
            ind2++;
        }
    }
    dir[ind1] = '\0';
    name[ind2] = '\0';
    if(dirname[0]=='/' && dir[0] == '\0')
    {
        dir[0] = '/';
        dir[1] = '\0';
    }
}


int do_ln(char *name_o, char *name_n) //硬链接
{
    //硬链接同一个node,从而同一个block
    sd_wr_superblock(READ);

    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return -1;
    }
    sd_wr_inode(READ);
    sd_wr_block(current_dir_entry->direct[0], READ);
        //切割名称和路径
    char name_old[3*MAX_NAME_LEN], dir_old[3*MAX_NAME_LEN];
    char name_new[3*MAX_NAME_LEN], dir_new[3*MAX_NAME_LEN];
    char dir_oldd[3][MAX_NAME_LEN];
    char dir_neww[3][MAX_NAME_LEN];
    int mode_o,mode_n;
    split_dir_name(name_old, dir_old, name_o);
    split_dir_name(name_new, dir_new, name_n);
    get_split_dir(dir_old, dir_oldd, &mode_o);
    get_split_dir(dir_new, dir_neww, &mode_n);
    //寻址
    inode_entry_t * old_trace, *new_trace;
    dir_entry_t *start_old, * start_new;
    start_old = find_path(dir_oldd, &old_trace, mode_o);
    start_new = find_path(dir_neww, &new_trace, mode_n);

    int i;
    for (i = 0; i < old_trace->file_size; i++)//old_trace只是一个目录，不是文件的
    {
        if (strcmp(start_old[i].name, name_old) == 0) //匹配到
        {
            if (start_old[i].type != TYPE_FILE)
                break;
            int inode_index;
            inode_index = start_old[i].inode_id;
            //添加链接
            strcpy(start_new[new_trace->file_size].name, name_new);
            start_new[new_trace->file_size].type = TYPE_FILE;
            start_new[new_trace->file_size].inode_id = inode_index;
            new_trace->file_size++;
            inode_entry[start_old[i].inode_id].reference++;//文件的链接数增加
        }
    }
    sd_wr_inode(WRITE);
    sd_wr_block(new_trace->direct[0], WRITE);
    return 0;
}

int do_ln_s(char *name_o, char *name_n) //软链接 
{
    //软链接不同inode，但是同一个block
    sd_wr_superblock(READ);

    if (super_block->magic != KFS_MAGIC)
    {
        prints("File system not exist !\n");
        return -1;
    }
    sd_wr_inode(READ);
    sd_wr_inodemap(READ);
    sd_wr_blockmap(READ);
    sd_wr_block(current_dir_entry->direct[0], READ);
    //切割名称和路径
    char name_old[3*MAX_NAME_LEN], dir_old[3*MAX_NAME_LEN];
    char name_new[3*MAX_NAME_LEN], dir_new[3*MAX_NAME_LEN];
    char dir_oldd[3][MAX_NAME_LEN];
    char dir_neww[3][MAX_NAME_LEN];
    int mode_o,mode_n;
    split_dir_name(name_old, dir_old, name_o);
    split_dir_name(name_new, dir_new, name_n);
    get_split_dir(dir_old, dir_oldd, &mode_o);
    get_split_dir(dir_new, dir_neww, &mode_n);
    //寻址
    inode_entry_t * old_trace, *new_trace;
    dir_entry_t *start_old, * start_new;
    start_old = find_path(dir_oldd, &old_trace, mode_o);
    start_new = find_path(dir_neww, &new_trace, mode_n);
    //
    int i;
    //匹配
    for (i = 0; i < old_trace->file_size; i++)
    {
        if (strcmp(start_old[i].name, name_old) == 0) //匹配到
        {
            if (start_old[i].type != TYPE_FILE)
                break;
            //添加inode，再添加链接
            int inode_index;
            inode_index = init_inode(0);
            //释放block，因为不用
            block_map[inode_entry[inode_index].direct[0]] = 0;
            //直接用cpy复制
            memcpy((void *)&inode_entry[inode_index], (void *)&inode_entry[start_old[i].inode_id], sizeof(inode_entry_t));
            inode_entry[inode_index].inode_id = inode_index;
            //填充
            strcpy(start_new[new_trace->file_size].name, name_new);
            start_new[new_trace->file_size].type = TYPE_FILE;
            start_new[new_trace->file_size].inode_id = inode_index;
            new_trace->file_size++;
        }
    }
    sd_wr_inode(WRITE);
    sd_wr_inodemap(WRITE);
    sd_wr_blockmap(WRITE);
    sd_wr_block(new_trace->direct[0], WRITE);
    return 0;
}

int fsearch(uint32_t fd, int mode, uint32_t locate )
{
    if(mode == 1 && fds[fd].mode!=O_RDONLY)//写
    {
        fds[fd].write_pos = locate;
        
        return 0;
    }
    else if(mode == 0 && fds[fd].mode!=O_WRONLY)//读
    {
        fds[fd].read_pos =locate;
        return 0;
    }
    else
    {
        prints("Can't find location %d",locate);
        return -1;
    }
}
