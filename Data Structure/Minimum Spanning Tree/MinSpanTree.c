#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <unistd.h>

#define MAX 450

typedef struct NetEdge
{                           //网的边，作为堆的节点
    int node1;              //第一个节点
    int node2;              //第二个节点
    int weight;             //边的权重
    struct NetEdge *lchild; //左孩子
    struct NetEdge *rchild; //右孩子
    struct NetEdge *parent; //双亲
} NetEdge, *LNetEdge;

typedef struct Array
{
    int node1;
    int node2;
    int weight;
} Array;

typedef struct MFset
{
    int XNum; //子集中的个数
    int x[30];
    int tag; //是否启用
} MFset;

typedef struct InsertNode
{
    int node1;
    int node2;
    int set1;
    int set2;
} InsertNode;

int NodeNum; //设成全局变量，方便调用
int EdgeNum; //全局变量，网的边数

LNetEdge CreateBiTree(Array s[MAX], int num)
{
    int ss = 0, front = 0, rear = 0;
    int n = num;
    Array d, d1, d2;
    LNetEdge t, bt, q;
    LNetEdge que[MAX];
    d = s[ss++];
    if (d.node1 != 0) //头结点
    {
        bt = (LNetEdge)malloc(sizeof(NetEdge));
        bt->node1 = d.node1;
        bt->node2 = d.node2;
        bt->weight = d.weight;
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
        if (d1.node1 == 0)
            break;

        q = (LNetEdge)malloc(sizeof(NetEdge));
        q->node1 = d1.node1;
        q->node2 = d1.node2;
        q->weight = d1.weight;
        t->lchild = q;
        q->rchild = q->lchild = NULL;
        que[rear++] = q;
        n--;

        d2 = s[ss++];
        if (d2.node1 == 0)
            break;
        q = (LNetEdge)malloc(sizeof(NetEdge));
        q->node1 = d2.node1;
        q->node2 = d2.node2;
        q->weight = d2.weight;
        t->rchild = q;
        q->rchild = q->lchild = NULL;
        que[rear++] = q;
        n--;
    }
    return bt;
}

void GetParent(LNetEdge p, LNetEdge pre)
{
    if (p != NULL)
    {
        GetParent(p->lchild, p);
        GetParent(p->rchild, p);
        p->parent = pre;
    }
}

void HeapAdjust(LNetEdge H)
{
    Array temp; //交换用的中间值
    //先序遍历，调整位置，让小的在上面
    if (H != NULL)
    {
        if ((H->lchild != NULL && H->rchild == NULL && H->weight > H->lchild->weight) || (H->lchild != NULL && H->rchild != NULL && H->weight >= H->lchild->weight && H->weight >= H->rchild->weight && H->lchild->weight <= H->rchild->weight) || (H->lchild != NULL && H->rchild != NULL && H->weight >= H->lchild->weight && H->weight <= H->rchild->weight)) //小于左边，右边不存在,左右存在，左边小于右边
        {
            temp.weight = H->weight;
            temp.node1 = H->node1;
            temp.node2 = H->node2;
            H->weight = H->lchild->weight;
            H->node1 = H->lchild->node1;
            H->node2 = H->lchild->node2;
            H->lchild->weight = temp.weight;
            H->lchild->node1 = temp.node1;
            H->lchild->node2 = temp.node2;
        }
        else if ((H->lchild == NULL && H->rchild != NULL && H->weight > H->rchild->weight) || (H->lchild != NULL && H->rchild != NULL && H->weight >= H->lchild->weight && H->weight >= H->rchild->weight && H->lchild->weight >= H->rchild->weight) || (H->lchild != NULL && H->rchild != NULL && H->weight <= H->lchild->weight && H->weight >= H->rchild->weight))
        {
            temp.weight = H->weight;
            temp.node1 = H->node1;
            temp.node2 = H->node2;
            H->weight = H->rchild->weight;
            H->node1 = H->rchild->node1;
            H->node2 = H->rchild->node2;
            H->rchild->weight = temp.weight;
            H->rchild->node1 = temp.node1;
            H->rchild->node2 = temp.node2;
        }
        HeapAdjust(H->lchild);
        HeapAdjust(H->rchild);
    }
}

void HeapInit(LNetEdge H)
{ //从最后开始，后续遍历
    if (H != NULL && !(H->lchild == NULL && H->rchild == NULL))
    {
        HeapInit(H->lchild);
        HeapInit(H->rchild);
        HeapAdjust(H);
    }
}

LNetEdge T1;
int flag; //flag为1的时候就返回
LNetEdge GetLeftorRight(LNetEdge T)
{
    if (T != NULL)
    {
        if (flag == 0)
            GetLeftorRight(T->lchild);
        if (T != NULL && T->lchild == NULL && T->rchild == NULL && flag == 0) //叶子结点
        {
            T1 = T;
            flag = 1;
        }
        if (flag == 0)
            GetLeftorRight(T->rchild);
    }
    return T1; //中序遍历的第一个一个叶子结点(最快)
}

Array HeapSort(LNetEdge H)
{
    int i;
    Array temp;
    LNetEdge LorR;
    //H都不为NULL，调用HeapSort的时候计数控制
    temp.node1 = H->node1;
    temp.node2 = H->node2;
    temp.weight = H->weight;
    if (!(H->lchild == NULL && H->rchild == NULL))
    {
        flag = 0;                 //换成0
        LorR = GetLeftorRight(H); //取最左边或最右边叶子结点
        //交换，中间保存
        H->node1 = LorR->node1;
        H->node2 = LorR->node2;
        H->weight = LorR->weight;
        if (LorR->parent->lchild == LorR)
            LorR->parent->lchild = NULL;
        else
            LorR->parent->rchild = NULL;
        free(LorR);
        HeapAdjust(H);
    }
    return temp;
}

void MFsetInit(MFset mf[31])
{
    int i;
    for (i = 1; i <= NodeNum; i++) //也是从1开始的
    {
        mf[i].tag = 1;
        mf[i].XNum = 1;
        mf[i].x[0] = i;
        mf[i].x[1] = 0;
    }
    for (; i <= 30; i++)
        mf[i].tag = 0;
    mf[0].tag = 0;
}

int InSameCntCop(MFset mf[31], InsertNode *BOUND)
{
    int i, j;
    int flag; //flag为0,1：不在同一个；flag为2：在同一个
    for (i = 1; i <= NodeNum; i++)
    {
        if (mf[i].tag == 1)
        {
            flag = 0; //每次进入一个新的连通分量，刷新
            for (j = 0; j < mf[i].XNum; j++)
            {
                if (mf[i].x[j] == (*BOUND).node1)
                {
                    flag++;
                    (*BOUND).set1 = i;
                }
                if (mf[i].x[j] == (*BOUND).node2)
                {
                    flag++;
                    (*BOUND).set2 = i;
                }
                if (flag == 2)
                    return 1;
            }
        }
    }
    return 0;
}

void InsertMF(MFset mf[31], InsertNode BOUND)
{ //将node2集合添加到node1的集合中，并删除node2
    //根据建立的时候的特性，node1<node2
    //应该是少的移动向多的
    int i, j;
    if (mf[BOUND.set1].XNum >= mf[BOUND.set2].XNum)
    {
        for (i = mf[BOUND.set1].XNum, j = 0; i < mf[BOUND.set1].XNum + mf[BOUND.set2].XNum, j < mf[BOUND.set2].XNum; i++, j++)
            mf[BOUND.set1].x[i] = mf[BOUND.set2].x[j]; //归并到set1中
        mf[BOUND.set1].XNum += mf[BOUND.set2].XNum;
        mf[BOUND.set2].tag = 0; //关闭
    }
    else
    {
        for (i = mf[BOUND.set2].XNum, j = 0; i < mf[BOUND.set2].XNum + mf[BOUND.set1].XNum, j < mf[BOUND.set1].XNum; i++, j++)
            mf[BOUND.set2].x[i] = mf[BOUND.set1].x[j]; //归并到set1中
        mf[BOUND.set2].XNum += mf[BOUND.set1].XNum;
        mf[BOUND.set1].tag = 0; //关闭
    }
}

int CplCnect(MFset mf[31]) //连通，则tag之和为1
{
    int i, tag = 0;
    for (i = 1; i <= NodeNum; i++)
        tag += mf[i].tag;

    if (tag == 1)
        return 1;
    else
        return 0;
}

LNetEdge HeapNetInit(/*Array array[MAX]*/)
{             //初始化，因为堆要是完全二叉树，故用层次遍历结果输入
    int mode; //1随机，2文件
A1:
    printf("************Choose the way to initiate Net: input number***********************************\n            [1]Random     [2]Input txt file\n");
    scanf("%d", &mode);
    int i, j, k = 0, c, temp, flag1 = 0; //前两个是表示节点，k计数
    EdgeNum = 0;
    Array array[MAX] = {'\0'};
    if (mode == 1)
    {
        srand((unsigned int)(time(NULL)));
        NodeNum = rand() % 31;
        //NodeNum = 30;
        printf("\n************The randomly generated nets are as follows:***********************\n");
        printf("The number of nodes is %d\n", NodeNum);
        for (i = 1; i <= NodeNum; i++)
        {
            for (j = i + 1; j <= NodeNum; j++, k++)
            { //j大于i
                array[k].node1 = i;
                array[k].node2 = j;
                array[k].weight = rand() % 100;
                EdgeNum += 1;
                printf("node1=%2d,node2=%2d,weight=%2d\n", array[k].node1, array[k].node2, array[k].weight);
            }
        }
        array[k].node1 = 0;
    }
    else if (mode == 2)
    {
        char filename[100];
        printf("\n************Please input file name, end with .txt **********************************\n");
        scanf("%s",filename);
        FILE *fp1=fopen(filename,"r");
        fscanf(fp1,"%d", &NodeNum);
        fgetc(fp1);
        MFset mf[31] = {'\0'}; //手动输入时要判断输入网是否连通
        InsertNode bound;
        MFsetInit(mf);
        while (1)
        {
            fscanf(fp1,"%d %d %d", &array[k].node1,&array[k].node2,&array[k].weight);
            //node1，node2都要小于NumNode
            if (array[k].node1 > NodeNum || array[k].node2 > NodeNum)
                flag1 = 1;
            //交换，保证node1小于node2
            if (array[k].node1 > array[k].node2)
            {
                temp = array[k].node1;
                array[k].node1 = array[k].node2;
                array[k].node2 = temp;
            }
            //判断是否为连通图
            bound.node1 = array[k].node1;
            bound.node2 = array[k].node2;
            if (!InSameCntCop(mf, &bound))
                InsertMF(mf, bound);

            EdgeNum += 1;
            k++;
            if ((c = fgetc(fp1)) != '\n')
                break;
        }
        //错1，输入节点数小于图的节点数
        if (flag1 == 1)
        {
            printf("\nThe number of node didn't match with the graph,please input again\n\n");
            goto A1;
        }
        //错2，输入网非连通
        if (!CplCnect(mf))
        {
            printf("\nThe original graph is unconnected, please input again\n\n");
            goto A1;
        }
        array[k].node1 = 0;
    }
    //利用层次遍历创建树,k是边节点的个数
    LNetEdge L;
    L = CreateBiTree(array, k);
    GetParent(L, NULL); //添加指向双亲
    HeapInit(L);        //堆初始化
    return L;
}

//连通与否——>数组结构不好->遍历
clock_t MinGeneratingTree(LNetEdge N /*Array N[MAX]*/, Array TREE[MAX]) //需要NodeNum-1条边
{
    clock_t start, finish;
    start = clock();
    Array MIN;
    MFset MF[31];
    InsertNode bound;
    int index = 0;
    MFsetInit(MF);
    while (!CplCnect(MF)) //图不是全连同则继续
    {
        MIN = HeapSort(N);
        bound.node1 = MIN.node1;
        bound.node2 = MIN.node2;
        if (!InSameCntCop(MF, &bound)) //不在同一个连通分量中
        {
            TREE[index++] = MIN;
            InsertMF(MF, bound);
        }
    }
    TREE[index].node1 = 0;
    finish = clock();
    return (double)(finish - start) / CLOCKS_PER_SEC;
}

void PrintTree(Array TREE[MAX], clock_t Time)
{
    printf("\n\n************The minimum spanning tree is as follows: *********************************\n");
    int index = 0;
    int Sum = 0;
    while (TREE[index].node1 != 0)
    {
        printf("Between city%2d and city%2d, length=%2d\n", TREE[index].node1, TREE[index].node2, TREE[index].weight);
        Sum += TREE[index].weight;
        index++;
    }
    printf("The total cost is %d\n", Sum);
    printf("The total generating time is %0.3f ms", Time);
}

int main()
{
    double time;
    system("color F0");
    LNetEdge Net; //树表示的网
A2: Net = HeapNetInit(/*nET*/); //初始化网，用“堆”来存网中的边
    Array Tree[MAX] = {'\0'};   //生成树的存储
    time = MinGeneratingTree(Net, Tree);
    PrintTree(Tree, time);
    //是否再来一次
    printf("\n\n*************Try again? input number*******************************************\n                 [1] YES     [2]NO\n");
    int flag2;
    scanf("%d",&flag2);
    if(flag2==1)
    goto A2;
    return 0;
}
