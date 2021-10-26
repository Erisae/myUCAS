//在 DFS 对图进行遍历时，将遍历过的顶点放入递归栈中，
//如果新遍历的顶点已经存在于递归栈中，则说明存在一个反向边，即存在一个环。
//即改写DFS
#include <stdio.h>
#include <stdlib.h>

#define MAX 10

typedef struct Graph
{
    int num;
    int s[MAX];
    int fst[MAX];
    int lst[MAX];
} Graph;

int visited[MAX + 1] = {'\0'};
int stack[MAX] = {'\0'};
int flag = 0; //flag指示的是是否有环

void get_s(Graph g, int node, int s[MAX])
{
    int i, j;
    for (i = g.fst[node], j = 0; i < g.lst[node]; i++, j++)
        s[j] = g.s[i];
}

int FirstAdjVex(Graph G, int Node)
{
    int s1[MAX] = {'\0'};
    if (G.fst[Node] > G.lst[Node])
        return -1; //表示没有后继节点
    get_s(G, Node, s1);
    return s1[0];
}

int NextAdjVex(Graph G, int Node, int W) //Node的下w的下一个
{
    int s1[MAX] = {'\0'};
    get_s(G, Node, s1);
    int i;
    for (i = 0; s1[i] != '\0'; i++) //先求W的下标
    {
        if (s1[i] == W)
            break;
    }
    if (s1[i + 1] == '\0')
        return -1;
    else
        return s1[i + 1];
}

void Push(int s[MAX], int Node)
{
    int i;
    for (i = 0; s[i] != '\0'; i++)
        ;
    s[i] = Node;
}

int exist(int s[MAX], int Node)
{
    int i = 0;
    for (i = 0; s[i] != '\0'; i++)
    {
        if (s[i] == Node)
            return 1;
    }
    return 0;
}

void Pop(int s[MAX])
{
    int i;
    for (i = 0; s[i] != '\0'; i++)
        ;
    s[i - 1] = '\0';
}

void DFS(Graph g, int node) //从node出发深度遍历
{
    int w;
    visited[node] = 1;
    Push(stack, node); //注意都要push,因为默认的是没访问过才访问
    for (w = FirstAdjVex(g, node); w >= 1 && flag == 0; w = NextAdjVex(g, node, w))
    {
        if (!visited[w])
            DFS(g, w);
        else //已经访问过了,但其实访问过了不一定就是环
        {
            if (exist(stack, w))//
                flag = 1;
        }
    }

    if (w < 0 && flag == 0)//如果该点相关都遍历完成，没有后继节点并且这时没有发现环，则pop最后一个出来
        Pop(stack);
}

void clearstack(int s[MAX])
{
    int i;
    for(i=0;i<MAX;i++)
    s[i]='\0';
}

int main()
{
    //输入
    Graph graph;
    scanf("%d\n", &graph.num);
    int i,c;
    i=0;
    while(1)
    {
        scanf("%d", &graph.s[i]);
        i++;
        if((c=getchar())=='\n')
        break;
    }
    for (i = 0; i <= graph.num; i++)
    {
        scanf("%d", &graph.fst[i]);
        getchar();
    }
    for (i = 0; i <= graph.num; i++)
    {
        scanf("%d", &graph.lst[i]);
        getchar();
    }
    //DFS
    for (i = 1; i < graph.num + 1; i++)
    {
        DFS(graph, i);
        clearstack(stack);
    }
    if (flag == 1)
        printf("yes");
    else
        printf("no");

    return 0;
}
/*
4
0 2 2 4 2
0 1 9 2 4
0 2 2 4 5
*/