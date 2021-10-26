/*
Description:
按照下述原则编写快排的非递归算法：

1. 一趟排序后，先对长度较短的子序列进行排序，且将另一子序列的上、下界入栈保存。

2. 若待排记录数<=3，则不再进行分割，而是直接进行比较排序。*/
#include <stdio.h>
#define size 100 //静态链表的容量
typedef int KeysType;
typedef struct
{
    KeysType key;
} SLNode;
typedef struct
{
    SLNode r[size];
    int length; //链表当前长度
} SLinkListType;

void GetIn(SLinkListType *L)
{
    int c, i;
    i = 1;
    (*L).length = 0;
    while (1)
    {
        scanf("%d", &c);
        (*L).length++;
        (*L).r[i++].key = c;
        if ((c = getchar()) == '\n')
            break;
    }
    (*L).r[0].key = (*L).r[1].key;
}


int stack[size]={'\0'};
int IsEmpty(int S[])
{
    if (S[0] == '\0')
        return 1;
    else
        return 0;
}

void push(int K)
{
    int i;
    for(i=0;stack[i]!='\0';i++)
    ;
    stack[i]=K;
    stack[i+1]='\0';
}

int pop()
{
    int i,num;
    for(i=0;stack[i]!='\0';i++);
    num=stack[i-1];
    stack[i-1]='\0';
    return num;
}

void QuikSort(SLinkListType *L)
{
    int low, high,lowre, highre,pivotloc;
    low = 1;
    high = (*L).length; //初始
    KeysType pivotkey, temp;
    do
    {
        if(low<0)//push
        {
            high=pop();
            low=pop();
        }
        if (high - low <= 2) //三个以下，比较排序
        {
            if (high - low == 2)
            {
                if ((*L).r[low].key > (*L).r[high].key)
                {
                    temp = (*L).r[low].key;
                    (*L).r[low].key = (*L).r[high].key;
                    (*L).r[high].key = temp;
                }
                if ((*L).r[low].key > (*L).r[low + 1].key)
                {
                    temp = (*L).r[low].key;
                    (*L).r[low].key = (*L).r[low + 1].key;
                    (*L).r[low + 1].key = temp;
                }
                if ((*L).r[low + 1].key > (*L).r[high].key)
                {
                    temp = (*L).r[low + 1].key;
                    (*L).r[low + 1].key = (*L).r[high].key;
                    (*L).r[high].key = temp;
                }
            }
            else if (high - low == 1)
            {
                if ((*L).r[low].key > (*L).r[high].key)
                {
                    temp = (*L).r[low].key;
                    (*L).r[low].key = (*L).r[high].key;
                    (*L).r[high].key = temp;
                }
            }
            low=-1;
            high=-1;//标记
        }
        else
        {
            (*L).r[0].key = (*L).r[low].key;
            pivotkey = (*L).r[low].key;
            lowre=low;
            highre=high;//保存原来的值
            while (low < high)//一次排序
            {
                while (low < high && (*L).r[high].key >= pivotkey)
                    high--;
                (*L).r[low].key = (*L).r[high].key;
                while (low < high && (*L).r[low].key <= pivotkey)
                    low++;
                (*L).r[high].key = (*L).r[low].key;
            }
            (*L).r[low]=(*L).r[0];
            pivotloc=low;

            if(highre-pivotloc-1<=pivotloc-1-lowre)
            {
                low=low+1;
                high=highre;
                push(lowre);
                push(pivotloc-1);
            }
            else
            {
                low=lowre;
                high=pivotloc-1;
                push(pivotloc+1);
                push(highre);
            }
        }
      } while (!IsEmpty(stack));
}

void printnum(SLinkListType L)
{
    int i;
    for(i=1;i<=L.length;i++)
    {
        if(i==1)
        printf("%d",L.r[i].key);
        else
        printf(",%d",L.r[i].key);
    }
}

int main()
{
    SLinkListType link;
    GetIn(&link);
    QuikSort(&link);
    printnum(link);

    return 0;
}
//12,45,4,26,25,88,10
