/*Description:
设顺序表va中的数据为递增有序。写一个算法，将x插入到顺序表的适当位置，保证有序性。
Input:
va顺序表，x值
Output:
插入x后va的结果
*/
#include <stdio.h>
#define MAX 1000
void inserts(int p[], int j, int k);
int locates(int x[], int g, int l);
int inputs(int[]);
int main()
{
    int j = 0, k, i;
    int a[MAX];
    i = inputs(a);
    k = a[i];
    a[i] = '\0';
    inserts(a, k, i);
    while (j<i)
    {
        printf("%d ", a[j]);
        j++;
    }
    printf("%d",a[j]);
    return 0;
}
int inputs(int p[])
{
    int j,c;
    for (j = 0; j < MAX; j++)
    {
        scanf("%d", &p[j]);
        if((c=getchar())=='\n')
        break;
    }
    return j;
}
void inserts(int p[], int j, int k)
{
    int i;
    int q = locates(p, j, k);
    for (i = k; i >= q; i--)
        p[i + 1] = p[i];
    p[q] = j;
}

int locates(int x[], int g, int l)
{
    int i;
    if (x[0] > g)
        return 0;
    else
    {
        for (i = 0; x[i] != '\0'; i++)
        {
            if (x[i] <= g  && g <= x[i + 1])
                return i + 1;
        }
        return l;
    }
}