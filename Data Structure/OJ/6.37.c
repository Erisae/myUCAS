//先序遍历的非递归形式
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//默认# 不会重复使用
typedef struct Node
{
	int data;
	struct Node *lchild, *rchild;
} BiNode, *LBiNode;

int GetArray(int *a)
{
	int c;
	int a1, a2;
	int i = 0;
	while ((a1=scanf("%d", &c))==1 || (a2=scanf("%c",&c))==1)
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

//栈操作函数
LBiNode Top (LBiNode a[100])
{
    int i=0;
    while(a[i]!=NULL)
    i++;
    return a[i-1];
}
void pop (LBiNode a[100])
{
    int i=0;
    while(a[i]!=NULL)
    i++;
    a[i-1]=NULL;
}

void push(LBiNode a[100],LBiNode TB)
{
    int i=0;
    while(a[i]!=NULL)
    i++;
    a[i]=TB;
    a[i+1]=NULL;
}
void bianli(LBiNode TT)
{
    LBiNode TA;
    LBiNode stack[100]={NULL};
    push(stack,TT);//在每次压栈的时候打印
    printf("%d",TT->data);
    TA=TT->lchild;
    while(stack[0]!=NULL || TA!=NULL)
    {
        while(TA!=NULL)//一直向左进栈直到没有左边了
        {
            push(stack,TA);
            printf(",%d",TA->data);
            TA=TA->lchild;
        }
        //后退到有右子树的上面
            while(stack[0]!=NULL && Top(stack)->rchild==NULL)
            pop(stack);
            if(stack[0]!=NULL)
            {
            TA=Top(stack)->rchild;
            pop(stack);
            }
    }
}

int main()
{
    //生成树
	int mytree[100];
	int len;
	len=GetArray(mytree);
	LBiNode T;
	T = CreateBiTree(mytree,len);
    //非递归遍历树
    bianli(T);
	return 0;
}
