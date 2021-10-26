//邻接矩阵
#include <stdio.h>
#define MAX 100

void init_graph(int g[MAX][MAX])
{
    int i, j;
    for (i = 0; i < MAX; i++)
    {
        for (j = 0; j < MAX; j++)
            g[i][j] = 0;
    }
}

int get_graph(int g[MAX][MAX])
{
    int node1, node2;
    int min_node = 1000;
    int c;
    while (1)
    {
        scanf("%d-%d", &node1, &node2); //最小的点，因为又可能是0，1
        min_node = min_node < node1 ? min_node : node1;
        min_node = min_node < node2 ? min_node : node2;
        g[node1][node2] = 1;
        if ((c = getchar()) == '\n') //读，或者‘\n'
            break;
    }
    return min_node;
}

void InsertVex(int *pnum) //增加点，返回点数
{
    *pnum = *pnum + 1;
}
int InsertArc(int g[MAX][MAX], int node1s, int node2s) //增加弧
{
    g[node1s][node2s] = 1;
}
void DeleteVex(int g[MAX][MAX], int node, int *pnum, int min) //删除点
{
    //n是原来的节点个数
    int i, j;
    for (i = min; i < min + *pnum - 1; i++)
    {
        for (j = min; j < min + *pnum - 1; j++)
        {
            if (i < node && j < node) //不变
                ;
            else if (i >= node && j >= node) //下面
                g[i][j] = g[i + 1][j + 1];
            else if (i < node && j >= node)
                g[i][j] = g[i][j + 1];
            else if (i >= node && j < node)
                g[i][j] = g[i + 1][j];
        }
    }
    *pnum = *pnum - 1;
}

void DeleteArc(int g[MAX][MAX], int node1s, int node2s)
{
    g[node1s][node2s] = 0;
}

void graph_act(int gg[MAX][MAX], int *num, int mmin)
{
    int node1, node2;
    int c;
    char c1, c2;
    while (1)
    {
        c1 = getchar();
        c2 = getchar();
        if (c1 == 'I' && c2 == 'V')
        {
            scanf("(%d)", &node1);
            InsertVex(num);
        }
        else if (c1 == 'I' && c2 == 'A')
        {
            scanf("(%d,%d)", &node1, &node2);
            InsertArc(gg, node1, node2);
        }
        else if (c1 == 'D' && c2 == 'V')
        {
            scanf("(%d)", &node1);
            DeleteVex(gg, node1, num, mmin);
        }
        else if (c1 == 'D' && c2 == 'A')
        {
            scanf("(%d,%d)", &node1, &node2);
            DeleteArc(gg, node1, node2);
        }
        if ((c = getchar()) == EOF)
            return;
    }
}


void print_graph(int g[MAX][MAX], int num, int min)
{
    int i, j;
    for (i = min; i < min + num; i++)
    {
        for (j = min; j < min + num; j++)
        {
            if (j != min + num - 1)
                printf("%d,", g[i][j]);
            else if (i != min + num - 1)
                printf("%d\n", g[i][j]);
            else
                printf("%d", g[i][j]);
        }
    }
}
int graph[MAX][MAX];
int main()
{
    //输入有向图的节点数目
    int node_num;
    scanf("%d", &node_num);
    getchar();
    //图

    //图初始化
    init_graph(graph);
    //图信息输入
    int min_node;
    min_node = get_graph(graph);
    //图操作
    graph_act(graph, &node_num, min_node);
    print_graph(graph, node_num, min_node);

    return 0;
}

/*
4
1-2,1-3,3-4,4-1
IV(5)
IA(5,2)
DV(2)

*/