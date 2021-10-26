//删除Trie树上的结点
//相较9.42添加删除函数
//trie树
#include <stdio.h>
#include <stdlib.h>
#define MAX 100
#define leaf 0
#define branch 1
typedef char KeysType;
typedef struct TrieNode
{
    int kind;
    union {
        struct
        {
            char info[MAX];
        } lf; //叶子节点
        struct
        {
            struct TrieNode *ptr[27];
            int num;
        } bh; //分支节点
    };
} TrieNode, *TrieTree;

void CreatTrie(TrieTree *t, char s[MAX])
{
    int c;
    int i, j, k;
    int flag = 0;
    TrieTree p, pre, next;
    while (1)
    {
        //获得字符串
        i = 0;
        while ((c = getchar()) != ';' && c != '\n')
            s[i++] = c;
        s[i] = '\0';
        if (c == '\n')
            break;
        //创建
        j = 0;
        while (s[j] != '\0')
        {
            if (j == 0 && flag == 0) //第一个结点第一次
            {
                (*t) = (TrieTree)malloc(sizeof(TrieNode));
                (*t)->kind = branch;
                (*t)->bh.num = 1;
                for (k = 0; k < 27; k++)
                    (*t)->bh.ptr[k] = NULL;
                pre = (*t);
            }
            else if (j == 0 && flag != 0) //第一个结点，之后
            {
                if ((*t)->bh.ptr[s[j] - 'a' + 1] != NULL)
                    (*t)->bh.num++;                  //无条件
                next = (*t)->bh.ptr[s[j] - 'a' + 1]; //没有是NULL,否则不是NULL
                pre = (*t);
            }
            else if (flag == 0) //之后结点，第一次
            {
                p = (TrieTree)malloc(sizeof(TrieNode));
                p->kind = branch;
                p->bh.num = 1;
                for (k = 0; k < 27; k++)
                    p->bh.ptr[k] = NULL;
                pre->bh.ptr[s[j - 1] - 'a' + 1] = p;
                pre = p;
            }
            else if (flag == 1) //之后结点，非第一次
            {
                if (next != NULL)
                {
                    next->bh.num++;
                    pre = next;
                    next = pre->bh.ptr[s[j] - 'a' + 1];
                }
                else
                {
                    p = (TrieTree)malloc(sizeof(TrieNode));
                    p->kind = branch;
                    p->bh.num = 1;
                    for (k = 0; k < 27; k++)
                        p->bh.ptr[k] = NULL;
                    pre->bh.ptr[s[j - 1] - 'a' + 1] = p;
                    pre = p;
                    next = NULL;
                }
            }
            j++;
        }

        if (flag == 1) //之后结点，非第一次
        {
            if (next == NULL)
            {
                p = (TrieTree)malloc(sizeof(TrieNode));
                p->kind = branch;
                p->bh.num = 1; //$
                for (k = 0; k < 27; k++)
                    p->bh.ptr[k] = NULL;
                pre->bh.ptr[s[j - 1] - 'a' + 1] = p;
            }
            else
            {
                p = next;
                p->bh.num++; //$增加
            }
        }
        else if (flag == 0) //之后结点，第一次
        {
            p = (TrieTree)malloc(sizeof(TrieNode));
            p->kind = branch;
            p->bh.num = 1;
            for (k = 0; k < 27; k++)
                p->bh.ptr[k] = NULL;
            pre->bh.ptr[s[j - 1] - 'a' + 1] = p;
        }
        pre = p;
        //结束，p的$指向结束叶子节点
        p = (TrieTree)malloc(sizeof(TrieNode));
        p->kind = leaf;
        for (i = 0; s[i] != '\0'; i++)
            p->lf.info[i] = s[i];
        p->lf.info[i] = s[i]; //'\0'
        pre->bh.ptr[0] = p;   //从$指过来

        flag = 1;
        //出循环
    }
}
TrieTree first;
int depth;
void printnum(TrieTree tree, int dep)
{
    int i;
    if (tree != NULL)
    {
        if (tree == first)
            printf("%d", tree->bh.num);
        else if (dep >= 1 && dep <= depth)
            printf(",%d", tree->bh.num);

        dep = dep + 1;
        for (i = 1; i < 27; i++)
            printnum(tree->bh.ptr[i], dep);
    }
}
char delete[MAX]={'\0'};
TrieTree GetPre(int num,TrieTree T)
{
    int i;
    for (i = 0; i < num-1 ; i++)//多了两个
        T = T->bh.ptr[delete[i] - 'a' + 1];

    return T;
}
/*
void DeleteKey(TrieTree *tree)
{
    //有没有的判断标准是$
    //num--：无条件；结点free:num==1;指针NULL:向下没有$,从下往上删除->递归
    int j, i;
    TrieTree pre,tree1,tree2;
    tree1=(*tree);//第一个记录
    for (j = 0; delete[j] != '\0'; j++)
        ;
    for (i = j; i >= 0; i--)
    {
        pre = GetPre(i+1,tree1); //前一个
        if(i==j)
        tree2 = pre->bh.ptr[0];
        else
        tree2 = pre->bh.ptr[delete[i] - 'a' + 1];

        if (tree2->kind == leaf || tree2->bh.num == 1) //删除结点，并让pre指向NULL
        {
            if(i==j)
            pre->bh.ptr[0] = NULL;
            else
            pre->bh.ptr[delete[i] - 'a' + 1] = NULL;
            free(tree2);
        }
        else //不删除结点，只减少num
            tree2->bh.num--;
    }
}
*/
//上述算法直接把结点和指针都删除了，但是看起来题目不想删结点和指针
void DeleteKey(TrieTree *tree)
{
    //有没有的判断标准是$
    //num--：无条件；结点free:num==1;指针NULL:向下没有$,从下往上删除->递归
    int j, i;
    TrieTree pre,tree1,tree2;
    tree1=(*tree);//第一个记录
    for (j = 0; delete[j] != '\0'; j++)
        ;
    for (i = j; i >= 0; i--)
    {
        pre = GetPre(i+1,tree1); //前一个
        if(i==j)
        tree2 = pre->bh.ptr[0];
        else
        tree2 = pre->bh.ptr[delete[i] - 'a' + 1];

        tree2->bh.num--;//只改变num
    }
}

int main()
{
    TrieTree T;
    scanf("%d;", &depth);
    CreatTrie(&T, delete);
    DeleteKey(&T);
    int i;
    for (i = 1; i < 27; i++)
    {
        if (T->bh.ptr[i] != NULL)
        {
            first = T->bh.ptr[i];
            break;
        }
    }
    int deep = 0;
    printnum(T, 0);
    return 0;
}
//3;apple;app;banana;banana
//3;trie;trie;search;start