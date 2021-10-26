#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define IMAGE_FILE "./image"
#define ARGS "[--extended] [--vm] <bootblock> <executable-file> ..."

#define SECTOR_SIZE 512
#define OS_SIZE_LOC 2
#define BOOT_LOADER_SIG_OFFSET 0x1fc
#define BOOT_LOADER_SIG_1 0x55
#define BOOT_LOADER_SIG_2 0xaa
#define BOOT_MEM_LOC 0x7c00
#define OS_MEM_LOC 0x1000

//structure to store command line options 
static struct
{
    int vm;
    int extended;
} options;

/* prototypes of local functions */
static void create_image(int nfiles, char *files[]);
//static void error(char *fmt, ...);
static void read_ehdr(Elf64_Ehdr *ehdr, FILE *fp);
static void read_phdr(Elf64_Phdr *phdr, FILE *fp, int ph,
                      Elf64_Ehdr ehdr);
static void write_segment(Elf64_Ehdr ehdr, Elf64_Phdr phdr, FILE *fp,
                          FILE *img, int *nbytes, int *first);
static void write_os_size(int nbytes, FILE *img);
//tatic void write_user_thread_segment(Elf64_Ehdr ehdr, Elf64_Phdr phdr, FILE *fp,
// FILE *img, int *nbytes, int *first);
FILE *img1;
int nbytes1, first1;
int main(int argc, char **argv)
{
    img1 = fopen(IMAGE_FILE, "w");
    create_image(argc, argv);
    printf("Image creating finished.\n");
    write_os_size(nbytes1, img1);
    fclose(img1);

    return 0;
}

static void create_image(int nfiles, char *files[])
{
    Elf64_Ehdr ehdr1;
    Elf64_Phdr phdr1;
    nbytes1 = 0;
    first1 = 1;

    for (int i = 2; i < nfiles; i++)
    {
        FILE *fp1 = fopen(files[i], "r"); //fopen(files[i], "a+");
        read_ehdr(&ehdr1, fp1);
        printf("0x%04lx: %s\n", ehdr1.e_entry, *files);
        for (int j = 0; j < ehdr1.e_phnum; j++)
        {
            read_phdr(&phdr1, fp1, j, ehdr1); //一个phdr指向一个segment
            write_segment(ehdr1, phdr1, fp1, img1, &nbytes1, &first1);
        }
        fclose(fp1);
    }
}

static void read_ehdr(Elf64_Ehdr *ehdr, FILE *fp)
{
    //认为开头就是ehdr
    fread(ehdr, sizeof(Elf64_Ehdr), 1, fp);
}

static void read_phdr(Elf64_Phdr *phdr, FILE *fp, int ph,
                      Elf64_Ehdr ehdr) //在table里面读
{
    fseek(fp, ehdr.e_phoff + ph * sizeof(Elf64_Phdr), SEEK_SET); //e_poff:table file offset；e_phentsize:table size 还是用Elf64_Phdr
    fread(phdr, sizeof(Elf64_Phdr), 1, fp);
}

static void write_segment(Elf64_Ehdr ehdr, Elf64_Phdr phdr, FILE *fp,
                          FILE *img, int *nbytes, int *first) //从phdr中找到segment的地址，然后读
{
    int sig = (phdr.p_memsz % 512 != 0);
    long all = phdr.p_memsz + sig * (512 - phdr.p_memsz % 512);
    char a[all]; //char型数组不能直接初始化
    *nbytes = all;
    for (int i = 0; i < all; i++)
    {
        a[i] = 0;
    }
    fseek(fp, phdr.p_offset, SEEK_SET); //p_offset是phdr指示的segemnt file的开始位置
    fread(a, phdr.p_filesz, 1, fp);
    fwrite(a, all, 1, img);
    if (*first == 1)
        *first = 0;

    /* 用追加的方式写
    char a[1];
    a[0]=0;
    char b[phdr.p_filesz];
    fseek(fp, phdr.p_offset, SEEK_SET);
    fread(b,phdr.p_filesz,1,fp);
    fwrite(b,phdr.p_filesz,1,fp);//程序段
    fwrite(a,phdr,p_memsz-phdr.p_filesz,1,fp);//把bss清0
    if (phdr.p_memsz % 512)
        fwrite(a, sizeof(char), 512 - phdr.p_memsz % 512, img);*/
}

static void write_os_size(int nbytes, FILE *img)
{
    wchar_t *a = (wchar_t *)malloc(sizeof(wchar_t));

    *a = (wchar_t)nbytes;
    fseek(img, BOOT_LOADER_SIG_OFFSET, SEEK_SET);
    fwrite(a, sizeof(wchar_t), 1, img);
    printf("The size of the kernel is %d bytes\n",nbytes);
    /*
    wchar_t a;
    a=(wchar_t)nbytes;
    *(wchar_t*)(img+BOOT_LOADER_SIG_OFFSET)=a;*/
}

/* print an error message and exit */
static void error(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    if (errno != 0) {
        perror(NULL);
    }
    exit(EXIT_FAILURE);
}
