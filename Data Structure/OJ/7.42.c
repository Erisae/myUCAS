//Dijkstra算法
#include <stdio.h>
#include <stdlib.h>
#define MAX 10
typedef struct ArcNode //邻接表节点
{
    int adjvex;
    struct ArcNode *nextarc;
    int value;
} ArcNode;

typedef struct VNode //顶点
{
    int data;
    ArcNode *firstarc;

} VNode, AdjList[MAX];

int get_list(AdjList H, int nn)
{
    //初始化H
    int i, value;
    for (i = 0; i < MAX; i++)
        H[i].firstarc = NULL;
    //输入弧
    int node_s, node_e; //一条弧的出发和终止
    ArcNode *p, *pp;

    while (1)
    {
        scanf("%d-%d %d", &node_s, &node_e, &value);
        //创建表节点
        p = (ArcNode *)malloc(sizeof(ArcNode));
        p->adjvex = node_e;
        p->value = value;
        p->nextarc = NULL;

        pp = H[node_s].firstarc; //这样的话应该保存最小的那个node_s,以便打印
        if (pp == NULL)
        {
            H[node_s].data = node_s;
            H[node_s].firstarc = p;
        }
        else
        {
            if (p->adjvex > pp->adjvex)
            {
                H[node_s].firstarc = p;
                p->nextarc = pp;
            }
            else
            {
                while (pp->nextarc != NULL && pp->nextarc->adjvex > p->adjvex)
                {
                    pp = pp->nextarc;
                }
                p->nextarc = pp->nextarc;
                pp->nextarc = p; //插入
            }
        }

        if ((i = getchar()) == EOF)
            break;
    }
    //将没有出发的节点数据变为该点相对应的值
    for (i = 1; i < 1 + nn; i++)
    {
        if (H[i].firstarc == NULL)
            H[i].data = i;
    }
}

int value(AdjList H1, int node1, int node2)
{
    ArcNode *p;
    p = H1[node1].firstarc;
    while (p != NULL)
    {
        if (p->adjvex == node2)
            return p->value;
        p = p->nextarc;
    }
    return 10000;
}

void DIJ(AdjList H, int node, int num, int D[MAX])
{
    int S[MAX] = {'\0'}, Path[MAX] = {'\0'};
    int i, w, v;
    int min;
    //初始化
    for (i = 1; i < num + 1; i++)
    {
        S[i] = 0;
        D[i] = value(H, node, i);
        if (D[i] < 10000)
            Path[i] = node;
        else
            Path[i] = -1;
    }
    S[node] = 1;
    D[node] = 0;
    //主循环
    for (i = 1; i < num; i++)
    {
        min = 10000;
        for (w = 1; w < num + 1; w++)
        {
            if (!S[w] && D[w] < min)
            {
                v = w;
                min = D[w];
            }
        }
        S[v] = 1;
        for (w = 1; w < num + 1; w++)
        {
            if (!S[w] && (D[v] + value(H, v, w) < D[w]))
            {
                D[w] = D[v] + value(H, v, w);
                Path[w] = v;
            }
        }
    }
}

void print(int D[MAX], int start, int num)
{
    int i;
    int flag = 0;
    for (i = 1; i < num + 1; i++)
    {
        if (flag == 0 && i!= start)
        {
            printf("%d", D[i]);
            flag = 1;
        }
        else if(i!=start)
        printf(",%d",D[i]);
    }
}

int main()
{
    AdjList L;
    //顶点数目
    int num_node, node_min, node_start;
    scanf("%d %d\n", &num_node, &node_start);
    //建立邻接表
    get_list(L, num_node); //最小的点编号是0还是1
    int D1[MAX] = {'\0'};
    DIJ(L, node_start, num_node, D1);
    print(D1, node_start, num_node);

    return 0;
}

/*
4 1
1-2 1
1-4 3
2-3 1
3-4 2
3-1 5
*/