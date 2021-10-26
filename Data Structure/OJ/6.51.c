//算术表达式，注意括号
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
//默认# 不会重复使用

int GetArray(int *a)
{
    int c;
    int a1 = 0, a2 = 0;
    int i = 0;
    while (1)
    {
        c = getchar();
        if (isdigit(c))
        {
            ungetc(c, stdin);
            a1 = scanf("%d", &c);
        }
        else
            a2 = 1;
        if (a1 || (a2 && c != 'n'))
            a[i] = c;
        else if (a2)
        {
            a[i] = '#';
            getchar();
            getchar();
            getchar();
        }
        i++;
        if ((c = getchar()) == '\n') //删去逗号
            break;
        a1 = 0;
        a2 = 0;
    }
    a[i] = '\0';
    return i;
}
typedef struct Node
{
    int data;
    struct Node *lchild, *rchild, *parent; //三叉链表
    int visit;
} BiNode, *LBiNode;

LBiNode CreateBiTree(int s[100], int num)
{
    int ss = 0, front = 0, rear = 0;
    int n = num;
    int d, d1, d2;
    LBiNode t, bt, q;
    LBiNode que[100];
    d = s[ss++];
    if (d != '#' && d != '\0')
    {
        bt = (LBiNode)malloc(sizeof(BiNode));
        bt->data = d;
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
        if (d1 != '#')
        {
            q = (LBiNode)malloc(sizeof(BiNode));
            q->data = d1;
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
        if (d2 != '#')
        {
            q = (LBiNode)malloc(sizeof(BiNode));
            q->data = d2;
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

void CreateBiTree_3(LBiNode TA)
{
    if (TA != NULL)
    {
        TA->visit = 0;
        if (TA->lchild != NULL)
            TA->lchild->parent = TA;
        if (TA->rchild != NULL)
            TA->rchild->parent = TA;
        CreateBiTree_3(TA->lchild);
        CreateBiTree_3(TA->rchild);
    }
}
LBiNode HEAD;
void print_tree(LBiNode H)
{
    if (H != NULL)
    {
        //中序遍历
        print_tree(H->lchild);
        //中间判断
        if (H->lchild != NULL && H->rchild != NULL) //说明H是符号
            printf("%c", H->data);
        else //说明是数据
        {
            if (H->parent->visit == 1) //要加括号
            {
                if (H->parent->lchild == H) //左边
                    printf("(%d", H->data);
                else if (H->parent->rchild == H) //右边
                    printf("%d)", H->data);
            }
            else
                printf("%d", H->data);
        }
        print_tree(H->rchild);
    }
}
void sign_tree(LBiNode H) //用先序遍历作标记
{
    if (H != NULL)
    {
        if (H->lchild != NULL && H->rchild != NULL && H != HEAD && (H->data == '+' || H->data == '-') && (H->parent->data == '*' || H->parent->data == '/' ||H->parent->data == '-'))
            H->visit = 1; //标记要加括号
        sign_tree(H->lchild);
        sign_tree(H->rchild);
    }
}

int main()
{
    //生成树
    int mytree[100] = {'\0'};
    int len;
    len = GetArray(mytree);
    LBiNode T, L1, L2;
    T = CreateBiTree(mytree, len);
    CreateBiTree_3(T);
    //打印
    HEAD = T;
    sign_tree(T);
    print_tree(T);

    return 0;
}
//*,1,+,null,null,3,6