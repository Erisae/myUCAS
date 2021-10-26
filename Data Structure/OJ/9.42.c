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

void CreatTrie(TrieTree *t)
{
    int c;
    char s[MAX] = {'\0'};
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
        if (c == '\n') //最后
            break;
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

int main()
{
    TrieTree T;
    scanf("%d;", &depth);
    CreatTrie(&T);
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
    printnum(T, deep);
    return 0;
}
//3;apple;app;banana 3;apple;app;banana
//3;trie;trie;search;start