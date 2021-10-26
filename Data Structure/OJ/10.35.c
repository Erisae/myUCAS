/*
假设定义堆为满足如下性质的完全三叉树：

1. 空树为堆

2. 根节点值不小于所有子树根的值，且所有子树均为堆(大于)

编写利用上述定义的堆进行排序的算法，输出从小到大的排序结果
*/
#include <stdio.h>
#include <stdlib.h>
#define MAX 100
typedef int KeysType;
typedef struct SqNode
{ //三叉树的节点
    KeysType key;
    struct SqNode *parent;
    struct SqNode *lchild;
    struct SqNode *rchild;
} SqNode, *SqList;
int GetArray(int *a) //这里不不能默认无#，该其为10000
{
    int c;
    int a1, a2;
    int i = 0;
    while ((a1 = scanf("%d", &c)) == 1 || (a2 = scanf("%c", &c)) == 1)
    {
        if (a1)
            a[i] = c;
        else if (a2)
        {
            a[i] = 10000;
            getchar();
            getchar();
            getchar();
        }
        i++;
        if ((c = getchar()) == '\n') //删去逗号
            break;
    }
    a[i] = '\0';
    return i;
}

SqList CreateBiTree(int s[100], int num)
{
    int ss = 0, front = 0, rear = 0;
    int n = num;
    int d, d1, d2;
    SqList t, bt, q;
    SqList que[100];
    d = s[ss++];
    if (d != 10000 && d != '\0')
    {
        bt = (SqList)malloc(sizeof(SqNode));
        bt->key = d;
        bt->rchild = bt->lchild = NULL;
        que[rear++] = bt;
        n--;
    }
    else
        return NULL;
    while (n)
    {
        t = que[front];
        front++;
        if (n == 0)
            break;
        d1 = s[ss++];
        if (d1 == '\0')
            break;
        if (d1 != 10000)
        {
            q = (SqList)malloc(sizeof(SqNode));
            q->key = d1;
            t->lchild = q;
            q->rchild = q->lchild = NULL;
            que[rear++] = q;
            n--;
        }
        else
            t->lchild = NULL;
        d2 = s[ss++];
        if (d2 == '\0')
            break;
        if (d2 != 10000)
        {
            q = (SqList)malloc(sizeof(SqNode));
            q->key = d2;
            t->rchild = q;
            q->rchild = q->lchild = NULL;
            que[rear++] = q;
            n--;
        }
        else
            t->rchild = NULL;
    }
    return bt;
}

void GetParent(SqList p, SqList pre)
{
    if (p != NULL)
    {
        GetParent(p->lchild, p);
        GetParent(p->rchild, p);
        p->parent = pre;
    }
}

void HeapAdjust(SqList H)
{
    int temp; //交换用的中间值
    //先序遍历，调整位置，让大的在上面
    if (H != NULL)
    {
        if ((H->lchild != NULL && H->rchild == NULL && H->key < H->lchild->key) || (H->lchild != NULL && H->rchild != NULL && H->key <= H->lchild->key && H->key <= H->rchild->key && H->lchild->key >= H->rchild->key) || (H->lchild != NULL && H->rchild != NULL && H->key <= H->lchild->key && H->key >= H->rchild->key)) //小于左边，右边不存在,左右存在，左边小于右边
        {
            temp = H->key;
            H->key = H->lchild->key;
            H->lchild->key = temp;
        }
        else if ((H->lchild == NULL && H->rchild != NULL && H->key < H->rchild->key) || (H->lchild != NULL && H->rchild != NULL && H->key <= H->lchild->key && H->key <= H->rchild->key && H->lchild->key <= H->rchild->key) || (H->lchild != NULL && H->rchild != NULL && H->key >= H->lchild->key && H->key <= H->rchild->key))
        {
            temp = H->key;
            H->key = H->rchild->key;
            H->rchild->key = temp;
        }
        HeapAdjust(H->lchild);
        HeapAdjust(H->rchild);
    }
}

void HeapInit(SqList H)
{ //从最后开始，后续遍历
    if (H != NULL && !(H->lchild == NULL && H->rchild == NULL))
    {
        HeapInit(H->lchild);
        HeapInit(H->rchild);
        HeapAdjust(H);
    }
}
SqList T1;
int flag = 0; //flag为1的时候就返回
SqList GetLeftorRight(SqList T)
{
    if (T != NULL)
    {
        if(flag==0)
        GetLeftorRight(T->lchild);
        if (T != NULL && T->lchild == NULL && T->rchild == NULL && flag==0) //叶子结点
        {
            T1 = T;
            flag = 1;
        }
        if(flag==0)
        GetLeftorRight(T->rchild);
    }
    return T1; //中序遍历的第一个一个叶子结点(最快)
}

void HeapSort(SqList H, int LEN, int S[MAX])
{
    int i;
    SqList LorR;
    for (i = 0; i < LEN; i++)
    {
        S[i] = H->key;
        if (!(H->lchild == NULL && H->rchild == NULL))
        {
            LorR = GetLeftorRight(H); //取最左边或最右边叶子结点
            H->key = LorR->key;
            if (LorR->parent->lchild == LorR)
                LorR->parent->lchild = NULL;
            else
                LorR->parent->rchild = NULL;
            free(LorR);
            HeapAdjust(H);
        }
    }
}

void PrintNum(int S[MAX], int LEN)
{
    int i;
    for (i = LEN - 1; i >= 0; i--)
    {
        if (i == LEN - 1)
            printf("%d", S[i]);
        else
            printf(",%d", S[i]);
    }
}

int main()
{
    //建立三叉树
    int len;
    int mytree[MAX] = {'\0'};
    len = GetArray(mytree);
    SqList L;
    L = CreateBiTree(mytree, len);
    GetParent(L, NULL);
    //堆初始化,从最后一个向上筛选
    HeapInit(L);
    int s[MAX];
    HeapSort(L, len, s); //s是排序之后的序列
    //打印
    PrintNum(s, len);
    return 0;
}
//12,45,4,26,25,88,10
//417,654,654,522,852,492,12,246,141,234,228,891,146,736,793,325,752,749,977,570,882,570,862,826
//12,45,4,26,26,88,10