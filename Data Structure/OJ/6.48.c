//求最近的共同祖先
//所有节点的左右子树交换
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//默认# 不会重复使用

int GetArray(int *a)//注意输入就是0的情况，把0看成字符
{
	int c;
	int a1, a2;
	int i = 0;
	while ((a1=scanf("%d", &c))==1 || (a2=scanf("%c",&c))==1)
	{
		if (a1)
        {   if(c==0)
            a[i] = -1;
            else 
			a[i] = c;
        }
		else if (a2)
		{
			a[i] = '#';
			getchar();
			getchar();
			getchar();
		}
		i++;
		if ((c = getchar()) == ';') //删去逗号
			break;
	}
	a[i] = '\0';
	return i;
}
typedef struct Node
{
	int data;
	struct Node *lchild, *rchild,*parent;//三叉链表
    int visit;
} BiNode, *LBiNode;

LBiNode CreateBiTree(int s[100],int num)
{
    int ss = 0,front = 0, rear = 0;
    int n = num;
    int d,d1,d2;
    LBiNode t, bt, q;
    LBiNode que[100];
    d = s[ss++];
    if(d != '#' && d != '\0'){
        bt = (LBiNode)malloc(sizeof(BiNode));
        bt->data = d;
        bt->rchild = bt->lchild = NULL;
        que[rear++] = bt;
        n--;
    }
    else
        return NULL;
    while(n){
        t = que[front];
        front++;
        if(n == 0)
            break;
        d1 = s[ss++];
        if(d1 == '\0')
            break;
        if(d1 != '#'){
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
        if(d2 == '\0')
            break;
        if(d2 != '#'){
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

void CreateBiTree_3(LBiNode TA)
{
    if(TA!=NULL)
    {
        TA->visit=0;
        if(TA->lchild!=NULL)
        TA->lchild->parent=TA;
        if(TA->rchild!=NULL)
        TA->rchild->parent=TA;
        CreateBiTree_3(TA->lchild);
        CreateBiTree_3(TA->rchild);
    }
}

void Get(LBiNode L, int a,LBiNode * LL)//先序
{
    if(L!=NULL)
    {
        if(L->data==a)
        *LL=L;
        Get(L->lchild,a,LL);
        Get(L->rchild,a,LL);
    }
}

void Get_np(LBiNode H,LBiNode h1,LBiNode h2)
{
    LBiNode H1=h1,H2=h2;
    while(H1!=H)
    {
        H1->visit++;
        H1=H1->parent;
    }
    while(H2!=H)
    {
        H2->visit++;
        H2=H2->parent;
    }
    H2=h2;
    while(H2!=H)
    {
        if(H2->visit==2)
        {
            if(H2->data==-1)
            printf("0");
            else
            printf("%d",H2->data);
            return;
        }
        H2=H2->parent;
    }
    printf("%d",H->data);
}

int main()
{
    //生成树
	int mytree[100]={'\0'};
	int len;
	len=GetArray(mytree);
	LBiNode T,L1,L2;
	T = CreateBiTree(mytree,len);
    CreateBiTree_3(T);

//获得两个点
    int c1,c2;
    scanf("%d",&c1);
    getchar();
    scanf("%d",&c2); 
    if(c1==0)
    c1=-1;
    if(c2==0)
    c2=-1;   
    Get(T,c1,&L1);
    Get(T,c2,&L2);
//获得最近的共同双亲
    Get_np(T,L1,L2);
    return 0;
}