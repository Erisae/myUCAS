#include <stdio.h>
#define MAX 100

void get_T(int L1[], int R1[], int T1[])
{
    int i = 1; //T1的下标
    int j = 1; //L,R中的下标
    while (i < MAX)
    {
        j = 1;
        while (j < MAX)
        {
            if (L1[j] == i || R1[j] == i)
            {
                T1[i] = j;
                break;
            }
            j++;
        }
        i++;
    }
}

int is_progeny(int T1[], int u1, int v1)
{
    if (u1 != 1 && v1 == 1)
        return 1;
    else
    {
        u1 = T1[u1];
        while (u1 != 1 && u1!=0)
        {
            if (u1 == v1)
                return 1;
            u1 = T1[u1];
        }
        return 0;
    }
}

int main()
{
    int L[MAX] = {'\0'};
    int R[MAX] = {'\0'};
    int i = 0;
    int c;
    int u;
    int v;
    //输入两个数组
    while (scanf("%d", &c) == 1)
    {
        L[i] = c;
        i++;
        if ((c = getchar()) == ';')
            break;
    }
    L[i] = '\0';
    i = 0;
    while (scanf("%d", &c) == 1)
    {
        R[i] = c;
        i++;
        if ((c = getchar()) == ';')
            break;
    }
    R[i] = '\0';
    scanf("%d", &u);
    getchar();
    scanf("%d", &v);

    //生成数组T
    int T[MAX] = {'\0'};
    get_T(L, R, T);
    printf("%d",is_progeny(T, u, v));

    return 0;
}

//0,2,0,0;0,3,0,0;2,1