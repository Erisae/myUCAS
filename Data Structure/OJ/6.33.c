//L R
//0,0,1,0,2,0,0,6;0,0,3,0,7,0,0,5;5;4
#include <stdio.h>
#define MAX 100

int is_progeny(int L1[], int R1[], int u1, int v1)
{
    if (v1 != 0)
    {
        if (L1[v1] == u1 || R1[v1] == u1)
            return 1;
        else
        {
            if (is_progeny(L1, R1, u1, L1[v1]))
                return 1;
            if (is_progeny(L1, R1, u1, R1[v1]))
                return 1;
        }
    }
    return 0;
}
int main()
{
    int L[MAX];
    int R[MAX];
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

    printf("%d", is_progeny(L, R, u, v));

    return 0;
}