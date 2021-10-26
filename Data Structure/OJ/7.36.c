//MPL域，求最长路径存到里面去
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
    int MPL;
} VNode, AdjList[100];

int get_list(AdjList H, int nn)
{
    //初始化H
    int i;
    for (i = 0; i < 100; i++)
    {
        H[i].firstarc = NULL;
        H[i].MPL = 0;
    }
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
                p->nextarc = pp->nextarc;
                pp->nextarc = p; //插入
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

void FindInDegree(AdjList E, int ind[], int numm, int minnn)
{
    int i;
    ArcNode *p;
    for (i = minnn; i < minnn + numm; i++)
    {
        p = E[i].firstarc;
        while (p != NULL)
        {
            ind[p->adjvex]++;
            p = p->nextarc;
        }
    }
}

void Push(int s[], int a)
{
    int i;
    for (i = 0; s[i] != '\0'; i++)
        ;
    s[i] = a;
}

void Pop(int s[], int *a)
{
    int i;
    for (i = 0; s[i] != '\0'; i++)
        ;
    *a=s[i-1];
    s[i-1]='\0';
}

void TopologicalSort(AdjList H, int topo[], int num, int minn)
{
    ArcNode *p;
    int k;
    int indegree[100] = {'\0'}; //每个点的入度
    FindInDegree(H, indegree, num, minn);

    int S[100] = {'\0'}; //栈
    int i;
    for (i = minn; i < minn + num; i++)
    {
        if (!indegree[i] && i != 0) //入度为0的入栈
            Push(S, i);
        else if (!indegree[i] && i == 0)
            Push(S, -1);
    }
    int m = 0; //对输出顶点计数
    while (!S[0] == '\0')
    {
        Pop(S, &i);
        topo[m] = i;
        m++;
        p = H[i].firstarc;
        while (p != NULL)
        {
            k = p->adjvex;
            indegree[k]--;
            if (indegree[k] == 0)
                Push(S, k);
            p = p->nextarc;
        }
    }
}

void print_MPL(AdjList H, int num, int minn)
{
    int i;
    for (i = minn; i < minn + num - 1; i++)
        printf("%d,", H[i].MPL-1);
    printf("%d", H[i].MPL-1);
}

void get_max_depth(AdjList HH, int Num, int Min)
{
    int Topo[100]={'\0'};
    TopologicalSort(HH,Topo,Num,Min);//获得拓扑排序
    int i;
    int max,temp;
    ArcNode *p;
    for(i=Num-1;i>=0;i--)//逆拓扑排序
    {
        p=HH[Topo[i]].firstarc;
        if(p==NULL)
        HH[Topo[i]].MPL=1;
        else
        {
           max=0;
           temp=0;
           while(p!=NULL)
           {
               if(HH[p->adjvex].MPL>max)
               {
                   max=HH[p->adjvex].MPL;
                   temp=p->adjvex;
               }
               p=p->nextarc;
           }
           HH[Topo[i]].MPL=max+1;
        }
    }
}

int main()
{
    AdjList L;
    //顶点数目
    int num_node, min; //顶点数目,最小值
    scanf("%d,%d", &num_node);
    getchar();
    //建立邻接表,创建的时候自动排序n
    min = get_list(L, num_node); //最小的点编号是0还是1
    //获得最小
    get_max_depth(L, num_node, min);
    print_MPL(L, num_node, min);

    return 0;
}
/*
6
1-2,2-3,3-4,5-2,5-6,6-4
*/

//DFS发现了bug，那就是深度优先搜索的顺序都是定好了的
//所以用拓扑排序的结果