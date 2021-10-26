//所有节点的左右子树交换
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//默认# 不会重复使用

int GetArray(int *a)
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
            a[i] = '#';
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
typedef struct Node
{
    int data;
    struct Node *lchild, *rchild;
} BiNode, *LBiNode;
LBiNode T;
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

void FOT_reverse(LBiNode TA)
{
    LBiNode TP;
    if (TA != NULL)
    {
        //先序遍历，把访问根节点变成交换根节点的左右子树
        TP = TA->lchild;
        TA->lchild = TA->rchild;
        TA->rchild = TP;
        FOT_reverse(TA->lchild);
        FOT_reverse(TA->rchild);
    }
}

void print_tree(LBiNode TA)
{
    static LBiNode TB;
    TB = TA;
    if (TA != NULL)
    {
        if (T == TA)
            printf("%d", TA->data);
        else
            printf(",%d", TA->data);

        print_tree(TA->lchild);
        print_tree(TA->rchild);
    }
}

int main()
{
    //生成树
    int mytree[100];
    int len;
    len = GetArray(mytree);
    T = CreateBiTree(mytree, len);
    FOT_reverse(T);
    print_tree(T);

    return 0;
}