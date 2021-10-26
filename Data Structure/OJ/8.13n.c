//边界标志法，首次适配策略
#include <stdio.h>
#include <stdlib.h>
#define FootLoc(p) p + p->size - 1
typedef struct WORD
{
    union {
        struct WORD *llink;
        struct WORD *uplink;
    };
    int head;//开头
    int tag;
    int size;
    struct WORD *rlink;
} WORD, *Space;

Space list_ini()
{
    int tag, start_addr, size;
    int c;
    Space p,pav; //上一个
    p = NULL;
    Space foot, first;
    while (1)
    {
        //输入
        scanf("%d %d %d\n", &tag, &start_addr, &size);
        //创建节点
        pav = (Space)start_addr;
        pav->tag = tag;
        pav->size = size;
        foot = FootLoc(pav); //获得的最后一个
        foot->uplink = pav;
        foot->tag = tag;
        //连接节点
        if (p == NULL) //第一个
        {
            pav->rlink = pav;
            pav->llink = pav;
            first = pav;
        }
        else
        {
            p->rlink = pav;
            pav->llink = p;
            first->llink = pav;
            pav->rlink = first;
        }
        p = pav;
        //判断循环时候结束
        if ((c = getchar()) == '\n')
            break;
        else
            ungetc(c,stdin);
    }
    return first;
}

int left_right_info(Space p, int s)
{
    Space left, right;
    left = p - 1;
    right = p + p->size;
    if (left->tag == 0 && right->tag == 0)
        return 1;
    else if (left->tag == 0 && right->tag == 1)
        return 2;
    else if (left->tag == 1 && right->tag == 0)
        return 3;
    else if (left->tag == 1 && right->tag == 1)
        return 4;
}
/*
Space get_back(Space pav) //注意这里面大量用了size所以要先赋值
{
    int tag, start_addr, size;
    int c;
    Space pa, foot, first;
    Space t, q, f, q1;
    int info;
    first = pav;
    while (1)
    {
        //输入
        scanf("%d %d %d", &tag, &start_addr, &size);
        //释放
        pa = start_addr;
        pa->size = size;
        pa->tag = tag;
        info = left_right_info(pa, size); //1 左右都是咱用快；2左空右占；
        if()
        switch (info)
        {                                   //3 左占右空；     ４左右都是空闲块
        case 1:
        {
            pa->tag = 0;
            foot = FootLoc(pa);
            foot->uplink = pa;
            foot->tag = 0;
            if (pav == NULL)
                pav = pa->llink = pa->rlink = pa;
            else
            {
                q = pav->llink;
                pa->rlink = pav;
                pa->llink = q;
                q->rlink = pav->llink = pa;
                pav = pa;
            }
            break;
        }
        case 2:
        {
            q = (pa - 1)->uplink; //左邻空闲快的头地址
            q->size += size;
            f = pa + size - 1;
            f->uplink = q;
            f->tag = 0;
            pav=pa;
            break;
        }
        case 3:
        {
            t = pa + pa->size;
            pa->tag = 0;
            q = t->llink;
            pa->llink = q;
            q->rlink = pa;
            f = t->rlink;
            pa->rlink = f;
            f->llink = pa;
            pa->size += t->size;
            f = FootLoc(t);
            f->uplink = pa;
            pav=pa;
            break;
        }
        case 4:
        {
            f = (pa - 1)->uplink;
            t = pa + pa->size;
            f->size += size + t->size;
            q = t->llink;
            q1 = t->rlink;
            q->rlink = q1;
            q1->llink = q;
            q = FootLoc(t);
            q->uplink = f;
            pav=pa;
            break;
        }
        default:
            break;
        }

        //判断
        if ((c = getchar()) == EOF)
            break;
    }
}
*/

int main()
{
    Space pav1, pav2;
    pav1 = list_ini();     //获得的是指向第一个节点的指针
    //pav2 = get_back(pav1); //释放内存，获得的是指向第一个节点的指针
    //printlist();//注意顺序 
    return 0;
}
/*
0 10000 15000
0 31000 8000
0 59000 41000

1 30000 1000
1 40000 2000
*/