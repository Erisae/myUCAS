//试编写算法，借助“计数”实现基数排序。
//借助计数，意味在基数排序的“排序”中用的是计数排序
#include <stdio.h>
#include <stdlib.h>
//定义新的数据类型
#define MAX_NUM_OF_KEY 3 //关键字项数的最大值
#define RADIX 10         //关键字基数
#define MAX_SPACE 10     //不要有头
typedef char KeysType;

typedef struct SLCell
{
    KeysType keys[MAX_NUM_OF_KEY]; //关键字
} SLCell;                          //静态链表的结点类型

typedef struct SLList
{
    SLCell r[MAX_SPACE]; //静态链表可用空间，r[0]为头节点
    int keynum;          //记录的当前关键字个数
    int recnum;          //静态链表当前长度
} SLList;

typedef int ArrType[RADIX]; //指针数组类型

void GetList(SLList *list)
{
    (*list).recnum = 10;
    (*list).keynum = 3;
    int i, j;
    for (i = 0; i < (*list).recnum; i++)
    {
        for (j = 0; j < (*list).keynum; j++)
            (*list).r[i].keys[j] = getchar();
        getchar();
    }
}

void RadixSort(SLList *list, int i)
{
    int s[MAX_SPACE] = {'\0'}; //辅助记录比他小的个数
    SLCell temp[MAX_SPACE];
    int j, k;
    for (j = 0; j < (*list).recnum; j++)
    {
        for (k = 0; k < (*list).recnum; k++)
        {
            if ((*list).r[j].keys[i] > (*list).r[k].keys[i])
                s[j]++;
            if ((*list).r[j].keys[i] == (*list).r[k].keys[i] && k < j) //相同的如果后出现啊加一
                s[j]++;
        }
    }
    //根据s[]重新排序
    for (j = 0; j < (*list).recnum; j++)
    {
        for (k = 0; k < (*list).keynum; k++)
            temp[s[j]].keys[k] = (*list).r[j].keys[k];
    }
    for (j = 0; j < (*list).recnum; j++)
    {
        for (k = 0; k < (*list).keynum; k++)
            (*list).r[j].keys[k] = temp[j].keys[k];
    }
}
void PrintNum(SLList list)
{
    int i, j;
    for (i = 0; i < list.recnum; i++)
    {
        if (i != 0)
            printf(" ");

        for (j = 0; j < list.keynum; j++)
            printf("%c", list.r[i].keys[j]);
    }
}


int main()
{
    SLList L; //采用静态链表表示的顺序表
    GetList(&L);

    int k;
    ArrType F, E;
    for (k = L.keynum - 1; k >= 0; k--)
        RadixSort(&L, k);
    PrintNum(L);
    return 0;
}
//210 410 162 530 640 384 505 269 010 760
//010 162 210 269 384 410 505 530 640 760