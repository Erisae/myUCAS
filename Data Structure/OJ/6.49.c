//完全二叉树定义，若设二叉树的深度为h，除第 h 层外，其它各层 (1～h-1) 的结点数都达到最大个数，
//第 h 层所有的结点都连续集中在最左边，这就是完全二叉树。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    int visit;//存储层次信息
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
int getdepth(LBiNode L)
{
    if (L == NULL)
        return 0;
    int left = getdepth(L->lchild) + 1;
    int right = getdepth(L->rchild) + 1;
    return left > right ? left : right;
}
int getallnum(LBiNode L)
{
    int nodes = 0;
    if (L == NULL)
        return 0;
    else
        nodes = getallnum(L->lchild) + getallnum(L->rchild) + 1; 
    return nodes;
}

int getlastnum(LBiNode L,int dep)
{
    int nodes = 0;
    if((L==NULL) || (L->lchild==NULL && L->rchild==NULL && L->visit!=dep))
    return 0;
    else if(L->lchild==NULL && L->rchild==NULL && L->visit==dep)
    return 1;
    else 
    {
        nodes=getlastnum(L->lchild,dep)+getlastnum(L->rchild,dep);
        return nodes;
    }
}

int dep_info(LBiNode H,int a)//附加层次信息
{
    if(H!=NULL)
    {
        H->visit=a;
        dep_info(H->lchild,a+1);
        dep_info(H->rchild,a+1);
    }
}

int correct(int Tree[], int f, int l)
{
    int i;
    for(i=f-1;i<f+l;i++)
    {
        if(Tree[i]=='#')
        return 0;
    }
    return 1;

}

int Judge_complete(LBiNode H,int tree[])
{
    int depth;
    int node_num,last_node_num,front_node_num;
    depth = getdepth(H); //数的层数
    node_num = getallnum(H);//总的节点的个数
    last_node_num=getlastnum(H,depth);//最后一层的节点个数
    front_node_num=node_num-last_node_num;//前面所有节点个数
    if((node_num==pow(2,depth)-1)||(front_node_num==(int)pow(2,depth-1)-1 && correct(tree,front_node_num,last_node_num)))
    return 1;
    else 
    return 0;

}
int main()
{
    //生成树
    int mytree[100]={'\0'};
    int len;
    len = GetArray(mytree);
    LBiNode T;
    T = CreateBiTree(mytree, len);
    dep_info(T,1);
    printf("%d", Judge_complete(T,mytree));

    return 0;
}

//4,2,6,null,1
