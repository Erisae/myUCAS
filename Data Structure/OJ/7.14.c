//有向图的邻接表
#include <stdio.h>
#include <stdlib.h>

typedef struct ArcNode //邻接表节点
{
    int adjvex;
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
    int node_s, node_e; //一条弧的出发和终止
    ArcNode *p, *pp;
    int min_node_s = 10000;

    while (1)
    {
        scanf("%d-%d", &node_s, &node_e);
        //创建表节点
        p = (ArcNode *)malloc(sizeof(ArcNode));
        p->adjvex = node_e;
        p->nextarc = NULL;
        min_node_s = (min_node_s < node_s) ? min_node_s : node_s;

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
                p->nextarc=pp->nextarc;
                pp->nextarc = p;//插入

            }
        }

        if ((i = getchar()) != ',')
            break;
    }
    //将没有出发的节点数据变为该点相对应的值
    for (i = min_node_s; i < min_node_s + nn; i++)
    {
        if (H[i].firstarc == NULL)
            H[i].data = i;
    }
    return min_node_s;
}
void print_list(AdjList H, int nn, int m)
{
    int i;
    ArcNode *p;
    for (i = m; i < nn + m; i++)
    {
        printf("%d", H[i].data);
        if (H[i].firstarc != NULL)
        {
            p = H[i].firstarc;
            while (p != NULL)
            {
                if (p == H[i].firstarc)
                    printf(" %d", p->adjvex);
                else
                    printf(",%d", p->adjvex);
                p = p->nextarc;
            }
        }
        if (i != nn + m - 1)
            printf("\n");
    }
}

int main()
{
    AdjList L;
    //顶点数目和弧数目输入
    int num_node, num_arc; //顶点数目，弧数目
    int min;
    scanf("%d", &num_node);
    getchar();
    scanf("%d", &num_arc);
    getchar();
    //建立邻接表,创建的时候自动排序
    min = get_list(L, num_node);
    //打印表关系
    print_list(L, num_node, min);
    return 0;
}
/*
5,12
0-3,0-1,1-4,1-2,1-0,2-4,2-3,2-1,3-2,3-0,4-2,4-1

4,6
1-4,2-1,1-3,2-4,2-3,3-4
*/