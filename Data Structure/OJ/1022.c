//十字链表
#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int i, j;
    int num;
    struct Node *right, *down;
    struct Node *left, *up;
} node, *pnode;

typedef struct head
{
    pnode rhead, dhead;
} CrossList;

typedef struct mtr_size
{
    int row, col;
} mtr_size;
void get_input(CrossList *L1, CrossList *L2, mtr_size *ss);
void get_list(CrossList L[100], mtr_size *s);
void add_mtrx(CrossList A[100], CrossList B[100], mtr_size s);
void printh(CrossList A[100], mtr_size s);

int main()
{
    CrossList M1[100], M2[100];
    mtr_size size;
    get_input(M1, M2, &size);
    add_mtrx(M1, M2, size);
    printh(M1, size);
    return 0;
}

void get_input(CrossList L1[100], CrossList L2[100], mtr_size *ss)
{
    //先获得size的大小
    int i, j;
    int c;
    scanf("%d", &c);
    ss->row = c;
    getchar();
    scanf("%d", &c);
    ss->col = c;
    getchar();
    getchar();
    getchar();
    //获得list L1 L2
    for (i = 0; i < 100; i++)
    {
        L1[i].rhead = NULL;
        L1[i].dhead = NULL;
        L2[i].dhead = NULL;
        L2[i].rhead = NULL;
    }
    get_list(L1, ss);
    getchar();
    getchar();
    get_list(L2, ss);
}
void get_list(CrossList L[100], mtr_size *s)
{
    int i, j;
    int c;
    pnode p, pp[100];
    for (i = 0; i < s->row; i++)
    {
        for (j = 0; j < s->col; j++)
        {
            scanf("%d", &c);
            if (c != 0)
            {
                p = (pnode)malloc(sizeof(node));
                p->i = i;
                p->j = j;
                p->down = NULL;
                p->right = NULL;
                p->num = c;
                if (L[i].rhead == NULL)
                {
                    L[i].rhead = p;
                    p->left = NULL;
                    pp[i] = p;
                }
                else
                {
                    p->left = pp[i];
                    pp[i]->right = p;
                    pp[i] = p;
                }

                if (L[j].dhead == NULL)
                {
                    L[j].dhead = p;
                    p->up = NULL;
                    pp[j] = p;
                }
                else
                {
                    p->up = pp[j];
                    pp[j]->down = p;
                    pp[j] = p;
                }
            }
            getchar();
        }
        getchar();
        getchar();
    }
}

void add_mtrx(CrossList A[100], CrossList B[100], mtr_size s)
{
    pnode pa, pb, px, pq;
    int i; //行
    for (i = 0; i < s.row; i++)
    {
        pa = A[i].rhead;
        pb = B[i].rhead;
        while (pa != NULL || pb != NULL)
        {
            if (pa == NULL || (pa != NULL && pb != NULL && (pa->j > pb->j))) //在a之前增加b节点
            {
                pq = pb->right; //暂存pb的值，否则之后移动pb就实际上在a上面了
                if (pa == NULL) //横向
                {
                    pa = A[i].rhead;
                    if (pa == NULL)
                    {
                        A[i].rhead = pb;
                        pb->left = NULL;
                        pb->right = pa;
                    }
                    else
                    {
                        while (pa->right != NULL)
                            pa = pa->right;
                        pa->right = pb;
                        pb->left = pa;
                        pb->left = NULL;
                        pa = pa->right;
                    }
                }
                else if (pa->left == NULL)
                {
                    A[i].rhead = pb;
                    pb->left = NULL;
                    pb->right = pa;
                    pa->left = pb;
                }
                else
                {
                    pa->left->right = pb;
                    pb->left = pa->left;
                    pb->right = pa;
                    pa->left = pb;
                }

                //纵向
                px = A[pb->j].dhead; //列的第一个
                if (px == NULL)//细节是真的多，，，
                {
                    A[i].dhead = pb;
                    pb->up = NULL;
                    pb->down = NULL;
                }
                else if (px->i > i)
                {
                    A[i].dhead = pb;
                    pb->up = NULL;
                    pb->down = px;
                    px->up = pb;
                }
                else
                {
                    while (px != NULL && px->i < i)
                        px = px->down;
                    if (px != NULL)
                    {
                        px = px->up; //插入到px的后面
                        pb->up = px;
                        pb->down = px->down;
                        px->down->up = pb;
                        px->down = pb;
                    }
                    else //插在最后的后面
                    {
                        px = A[pb->j].dhead;
                        while (px->down != NULL)
                            px = px->down; //插入px的后面
                        px->down = pb;
                        pb->up = px;
                        pb->down = NULL;
                    }
                }
                pb = pq;
            }
            else if (pb == NULL || (pa != NULL && pb != NULL && (pa->j < pb->j))) //pa,pb!=NULL is a must
                pa = pa->right;
            else if (pa->j == pb->j)
            {
                if (pa->num + pb->num == 0) //删除节点
                {
                    //横向
                    if (pa->left == NULL)
                    {
                        A[i].rhead = pa->right;
                        pa->right->left = NULL;
                    }
                    else
                    {
                        pa->left->right = pa->right;
                        pa->right->left = pa->left;
                    }
                    //纵向
                    if (pa->up == NULL) 
                    {
                        A[i].dhead = pa->down;
                        pa->down->up = NULL;
                    }
                    else
                    {
                        pa->up->down = pa->down;
                        pa->down->up = pa->up;
                    }
                    free(pa);
                }
                else
                    pa->num = pa->num + pb->num;
                pa = pa->right;
                pb = pb->right;
            }
        }
    }
}

void printh(CrossList A[100], mtr_size s)
{
    int i, j;
    pnode a;
    printf("[[");
    for (i = 0; i < s.row; i++)
    {
        a = A[i].rhead;
        for (j = 0; j < s.col; j++)
        {
            if (a != NULL && a->j == j)
            {
                if (j == s.col - 1)
                    printf("%d]", a->num);
                else
                    printf("%d,", a->num);
                a = a->right;
            }
            else
            {
                if (j == s.col - 1)
                    printf("0]");
                else
                    printf("0,");
            }
        }
        if (i == s.row - 1)
            printf("]");
        else
            printf(";[");
    }
}
