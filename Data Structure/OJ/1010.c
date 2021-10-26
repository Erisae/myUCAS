/*Description:
已知 A，B和 C为三个递增有序的线性表，
现要求对 A表作如下操作：删去那些既在B表中出现又在C表中出现的元素。
Input:
输入数据中，按照A，B和C的顺序输入，其中用分号分开，即A;B;C
Output:
顺序输出删除后，A剩余的元素。*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

struct node
{
    int num;
    struct node *next;
};
int getin(struct node *px1, struct node *py1, struct node *pz1)
{
    int k;
    struct node *p, *p1, *px, *py, *pz;
    int g, c;
    px = NULL;
    py = NULL;
    pz = NULL;
    while ((k=(scanf("%d", &g) == 1))||scanf("%c", &g) == 1 )
    {
        p = (struct node *)malloc(sizeof(struct node *));
        p->num = g;
        p->next = NULL;
        if (px == NULL)
            px = p;
        else
            p1->next = p;
        p1 = p;
        if ((c = getchar()) == ';' || c == '\n')
            break;
    }
    while (scanf("%d", &g) == 1 || scanf("%c", &g) == 1 )
    {
        p = (struct node *)malloc(sizeof(struct node *));
        p->num = g;
        p->next = NULL;
        if (py == NULL)
            py = p;
        else
            p1->next = p;
        p1 = p;
        if ((c = getchar()) == ';' || c == '\n')
            break;
    }
    while (scanf("%d", &g) == 1||scanf("%c", &g) == 1 )
    {
        p = (struct node *)malloc(sizeof(struct node *));
        p->num = g;
        p->next = NULL;
        if (pz == NULL)
            pz = p;
        else
            p1->next = p;
        p1 = p;
        if ((c = getchar()) == ';' || c == '\n')
            break;
    }
    *px1 = *px;
    *py1 = *py;
    *pz1 = *pz; //为什么不能传指针 只能传地址？
    return k;
}
struct node *gen(struct node *x, struct node *y, struct node *z)
{
    struct node *px, *ppx, *pppx, *py, *pz, *q;
    int i;
    px = x;
    py = y;
    pz = z;
    if (px->num == py->num && py->num == pz->num)
    {
        x = x->next;
        py = py->next;
        pz = pz->next;
    }
    px = x;
    while (px != NULL)
    {
        ppx = x;
        if (py != NULL && pz != NULL)
        {
            while ((py != NULL && pz != NULL) && ((px->num >= py->num && px->num > pz->num) || (px->num > py->num && px->num >= pz->num)))
            {
                py = py->next;
                pz = pz->next;
            }
            while ((py != NULL && pz != NULL) && ((px->num <= py->num && px->num > pz->num) || (px->num < py->num && px->num >= pz->num)))
                pz = pz->next;
            while ((py != NULL && pz != NULL) && ((px->num >= py->num && px->num < pz->num) || (px->num > py->num && px->num <= pz->num)))
                py = py->next;
            if (pz!=NULL && py !=NULL && px->num == py->num && px->num == pz->num)
            {
                while ((ppx->next) != px)
                    ppx = ppx->next;
                if (ppx->next->next == NULL)
                {
                    ppx->next = NULL;
                    break;
                }
                else
                {
                    for (q = ppx->next, i = 0; ppx->next->num == q->num; q = q->next, i++)
                        ;
                    px = q;
                    ppx->next = px;
                    //pppx = px;
                    py = py->next;
                    pz = pz->next;
                    //free(pppx);
                }
            }
            else
                px = px->next;
        }
        else
            px = px->next;
    }

    return x;
}

void prints(struct node *q,int c)
{
    if(c!=1)
    {
    while (q->next != NULL)
    {
        printf("%c,", q->num);
        q = q->next;
    }
    printf("%c", q->num);
}
else
{
    while (q->next != NULL)
    {
        printf("%d,", q->num);
        q = q->next;
    }
    printf("%d", q->num);

}

}

int main()
{
    struct node pa, pb, pc;
    struct node *p;
    int x;
    x=getin(&pa, &pb, &pc); //输入
    p = gen(&pa, &pb, &pc);
    prints(p,x);
    return 0;
}