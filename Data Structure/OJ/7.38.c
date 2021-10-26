//存的是表达式，输出逆波兰
#include <stdio.h>
#include <stdlib.h>

typedef struct ArcNode //邻接表节点
{
    char adjvex;
    struct ArcNode *nextarc;
} ArcNode;

typedef struct VNode //顶点
{
    int data;
    ArcNode *firstarc;
} VNode, AdjList[100];

int get_list(AdjList H, int nn)
{
    //初始化H
    int i;
    for (i = 0; i < 100; i++)
        H[i].firstarc = NULL;
    //输入弧
    int node1, node2, temp; //一条弧的出发和终止
    char node_in;
    ArcNode *p1, *p2;
    int c;
    for (i = 0; i < nn; i++)
    {
        //输入第一个符号
        node_in = getchar();
        H[i].data = node_in;
        if ((c = getchar()) == '\n')
            continue;
        //输入后两个
        scanf("%d %d\n", &node1, &node2);
        //创建表节点
        p1 = (ArcNode *)malloc(sizeof(ArcNode));
        p1->adjvex = node1;
        p2 = (ArcNode *)malloc(sizeof(ArcNode));
        p2->adjvex = node2;
        H[i].firstarc = p1;
        p1->nextarc = p2;
        p2->nextarc = NULL;
    }
}
int visited[100] = {'\0'};
void print_f(AdjList H, int node_now)
{
    ArcNode *p;
    int w;
    p = H[node_now].firstarc;
    visited[node_now] = 1;
    if (p == NULL)
        printf("%c", H[node_now].data);
    else
    {
        while (p != NULL)
        {
            w = p->adjvex;
            if (!visited[w])
                print_f(H, w);
            p = p->nextarc;
        }
        printf("%c", H[node_now].data);
    }
}

int change(AdjList H1, int node)//使出来是这样 即一个数一个表达式则换位置，但并不知道为什么
{
    int a1, a2;
    a1 = H1[node].firstarc->adjvex;
    a2 = H1[node].firstarc->nextarc->adjvex;
    if ((H1[a1].firstarc != NULL && H1[a2].firstarc == NULL) || (H1[a2].firstarc != NULL && H1[a1].firstarc == NULL))
        return 1;
    else if (H1[a1].data > H1[a2].data)
        return 1;
    else
        return 0;
}

void change_oder(AdjList H, int num)
{
    ArcNode *temp;
    int i;
    for (i = 0; i < num; i++)
    {
        if (H[i].firstarc != NULL && change(H, i) == 1)
        {
            temp=H[i].firstarc->nextarc;
            H[i].firstarc->nextarc->nextarc=H[i].firstarc;
            H[i].firstarc->nextarc=NULL;
            H[i].firstarc=temp;
        }
    }
}

int main()
{
    AdjList L;
    //顶点数目
    int num_node;
    scanf("%d\n", &num_node);
    //建立邻接表,创建的时候自动排序,从小到大
    get_list(L, num_node);
    change_oder(L, num_node);
    print_f(L, 0); //打印逆波兰表达式

    return 0;
}
/*
5 
* 2 1
+ 4 3
c
a
b
*/
