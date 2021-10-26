//邻接表，深度优先遍历
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
int visited[100]={'\0'};//visited数组
void DFS(AdjList L,int node_start)
{
    ArcNode * p;
    int w;
    visited[node_start]=1;
    p=L[node_start].firstarc;
    while(p!=NULL)
    {
        w=p->adjvex;
        if(!visited[w])
        DFS(L,w);
        p=p->nextarc;

    }
}

int main()
{
    AdjList L;
    //顶点数目和弧数目输入
    int num_node; //顶点数目，弧数目
    int min;
    scanf("%d", &num_node);
    getchar();
    //建立邻接表,创建的时候自动排序
    min = get_list(L, num_node);//最小的点编号是0还是1
    //输入开始和结束的点
    int node1,node2;
    scanf("%d,%d",&node1,&node2);
    //DFS,深度优先搜索
    DFS(L,node1);
    if(visited[node2]==1)
    printf("yes");
    else printf("no");
    return 0;
}
/*
6
1-2,2-3,5-6,5-2,6-4,3-4
5,4
*/