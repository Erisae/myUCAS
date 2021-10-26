//广义表
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    ATOM,
    LIST
} Elemtag;

typedef struct GLNode
{
    Elemtag tag; //用以标记是单个元素或广义表
    union {
        char atom[10];
        struct
        {
            struct GLNode *sonlist;
            struct GLNode *next;
        } p;
    } re;
} GLNode, *GList;

void CreatGList(GList *List, char str[100]);
void sever(char ss[100], char ssh[100]);
int isint(char k[]);
void clearstr(char k[100]);
void substring(char s_get[], char s_org[], int start_index, int num);
int strempty(char s1[]);
int strcompare(char s1[], char s2[]);
int GListPrint(GList G);
int DeletAtom(GList *List, char xx[10]);
//用递归的方法创建广义表，并在该过程中删除 x（x是输入的）
//全局变量
int main()
{
    char x[10] = {'\0'};
    char s[100] = {'\0'};
    char temp[100] = {'\0'};
    int i, j;
    int flag; //旗子，标志x是“数据”（1）还是“字符”（0）
    scanf("%s", temp);
    for (i = 0; temp[i] != ';'; i++)
        x[i] = temp[i];
    x[i++] = '\0';
    for (j = 0; temp[i] != '\0'; i++, j++)
        s[j] = temp[i];
    s[j] = '\0';
    //生成链表
    GList L;
    CreatGList(&L, s);
    DeletAtom(&L, x);
    GListPrint(L);
    return 0;
}

int DeletAtom(GList *List, char xx[10]) //递归删除广义表L中的原子项x
{                                       //删除原子项x但不删除产生的空表
    if (List == NULL)
        return 0; //空表
    else
    {
        if ((*List)->tag == ATOM)
        { //原子类型
            if (strcompare((*List)->re.atom, xx))
            {
                (*List)->re.atom[0] = '\0';
            }
        }
        else
        { //子广义表
            GLNode *ptr = (*List);
            while (ptr)
            {
                DeletAtom(&(ptr->re.p.sonlist), xx);
                ptr = ptr->re.p.next;
            }
        }
    }
    return 0;
}
void CreatGList(GList *List, char str[100])
{
    GList p, q;
    char emp[] = "()";
    char sub[100], hsub[100];
    if (strcompare(str, emp))
        *List = NULL;
    else
    {
        (*List) = (GList)malloc(sizeof(GLNode));
        if ((strlen(str) == 1 && isalpha(str[0])) || ((strlen(str) == 1 && isdigit(str[0])) || isint(str))) //是字符
        {
            (*List)->tag = ATOM;
            strcpy((*List)->re.atom, str);
        }
        else
        {
            (*List)->tag = LIST;
            p = *List;
            substring(sub, str, 2, strlen(str) - 2); //脱括号
            do
            {                                         //重复建立
                sever(sub, hsub);                     //bug              //从sub中分离出表头的串hsub,sub里面就是表尾
                CreatGList(&(p->re.p.sonlist), hsub); //子表
                q = p;
                if (!strempty(sub))
                { //表尾不空
                    p = (GList)malloc(sizeof(GLNode));
                    p->tag = LIST;
                    q->re.p.next = p;
                }
            } while (!strempty(sub));
            q->re.p.next = NULL;
        }
    }
}

void sever(char ss[100], char ssh[100])
{
    int i, k, n;
    n = strlen(ss);
    i = 0;
    k = 0;
    char ch[2]; //避免内存溢出
    do
    {
        ++i;
        substring(ch, ss, i, 1);
        if (ch[0] == '(')
            ++k;
        else if (ch[0] == ')')
            --k;
    } while (i < n && (ch[0] != ',' || k != 0));
    if (i < n)
    {
        substring(ssh, ss, 1, i - 1);
        substring(ss, ss, i + 1, n - i);
    }
    else
    {
        strcpy(ssh, ss);
        clearstr(ss);
    }
}

int isint(char k[])
{
    int i;
    for (i = 0; k[i] != '\0'; i++)
    {
        if (!isdigit(k[i]))
            return 0;
    }
    return 1;
}

void clearstr(char k[100])
{
    int i;
    for (i = 0; i < 100; i++)
        k[i] = '\0';
}

void substring(char s_get[], char s_org[], int start_index, int num)
{
    int i, j;
    for (i = start_index - 1, j = 0; j < num; i++, j++)
        s_get[j] = s_org[i];
    s_get[j] = '\0';
}

int strempty(char s1[])
{
    int i;
    if (strlen(s1) == 0)
        return 1;
    else
        return 0;
}

int strcompare(char s1[], char s2[])
{
    int i;
    if (strlen(s1) != strlen(s2))
        return 0;
    else
    {
        for (i = 0; s1[i] != '\0' && s2[i] != '\0'; i++)
        {
            if (s1[i] != s2[i])
                return 0;
        }
        return 1;
    }
}

//打印广义表
int GListPrint(GList L) //递归打印广义表
{
    if (L == NULL)
        printf("()");
    else
    {
        if (L->tag == ATOM)
        {                              //原子类型
            if (L->re.atom[0] != '\0') //不然不打印
                printf("%s", L->re.atom);
        }
        else
        { //子广义表，递归打印
            printf("(");
            GLNode *ptr = L;
            while (ptr)
            {
                GListPrint(ptr->re.p.sonlist);
                if (ptr->re.p.sonlist->re.atom[0] != '\0' && ptr->re.p.next)
                { //当有后继结点时
                    if (!(ptr->re.p.next && ptr->re.p.next->re.p.sonlist->re.atom[0] == '\0' && !ptr->re.p.next->re.p.next))
                        printf(",");
                }
                ptr = ptr->re.p.next;
            }
            printf(")");
        }
    }
    return 0;
}
