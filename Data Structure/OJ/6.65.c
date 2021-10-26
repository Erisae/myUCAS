#include <stdio.h>
#include <stdlib.h>
//利用前序和中序来建立二叉链表
//递归的思想，找根节点，划分左右子树，子树递归
typedef struct Node
{
    int data;
    struct Node *lchild, *rchild;
} BiNode, *LBiNode;

void getin(int F[], int M[])
{
    int c;
    int i = 0;
    while (1)
    {
        scanf("%d", &c);
        F[i] = c;
        i++;
        if ((c = getchar()) == ';')
            break;
    }
    F[i] = '\0';
    i = 0;
    while (1)
    {
        scanf("%d", &c);
        M[i] = c;
        i++;
        if ((c = getchar()) == '\n')
            break;
    }
    M[i] = '\0';
}

void getslices(int f[], int m[], int fl[], int fr[], int ml[], int mr[])
{
    int i = 0;
    int j = 0;
    int x;
    while (m[i] != f[0])
    {
        ml[i] = m[i];
        i++;
    }
    x = i;
    i++;
    while (m[i] != '\0')
    {
        mr[j] = m[i];
        i++;
        j++;
    }
    i = 1;
    j = 0;
    while (i <= x)
    {
        fl[j] = f[i];
        j++;
        i++;
    }
    j = 0;
    while (f[i] != '\0')
    {
        fr[j] = f[i];
        i++;
        j++;
    }
}

void CreateTree(int F[], int M[], LBiNode *H) //一个是先序遍历一个是中序遍历
{
    if (F[0] != '\0')
    {
        int Fl[100] = {'\0'}, Fr[100] = {'\0'}, Ml[100] = {'\0'}, Mr[100] = {'\0'};
        *H = (LBiNode)malloc(sizeof(BiNode));
        (*H)->data = F[0];
        (*H)->lchild = NULL;
        (*H)->rchild = NULL;
        getslices(F, M, Fl, Fr, Ml, Mr);
        CreateTree(Fl, Ml, &((*H)->lchild));
        CreateTree(Fr, Mr, &((*H)->rchild));
    }
}
LBiNode First;//第一个点

void GetFirst(LBiNode H)
{
    if(H->lchild==NULL)
    H=H->rchild;
    while(H->lchild!=NULL)
        H=H->lchild;
    if(H->rchild!=NULL)
        H=H->rchild;
    First=H;
    
}

void print_tree(LBiNode H)
{
    if (H != NULL)
    {
        print_tree(H->lchild);
        print_tree(H->rchild);
        if (H == First)
            printf("%d", H->data);
        else
            printf(",%d", H->data);
    }
}

int main()
{
    //输入
    int frt[100] = {'\0'}, med[100] = {'\0'};
    getin(frt, med);
    //递归建立
    LBiNode T; //根节点
    CreateTree(frt, med, &T);
    GetFirst(T);
    print_tree(T);
    return 0;
}
//3,9,20,15,7;9,3,15,20,7