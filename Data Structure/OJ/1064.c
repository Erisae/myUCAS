/*Description:
试编写算法求一元多项式
Pn(x)=a0x0+a1x1+a2x2+...+anxn的值，其中，i=0,1,2,...,n
Input:
首先输入n和x，以空格分隔
然后接下来输入n个数字代表ai,以空格分隔
Output:
输出Pn(x)的值*/
#include <stdio.h>
#include <math.h>
int calculate(int[], int, int);
int main()
{
    int n, x, i, c, g;
    int a[100];
    scanf("%d %d", &n, &x);

    for (i = 0; i < n; i++)
    {
        scanf("%d", &a[i]);
        if ((g = getchar()) == '\n')
            break;
    }
    c = calculate(a, n, x);
    printf("%d", c);
    return 0;
}
int calculate(int q[], int w, int e)
{
    int k = 0, j;
    for (j = 0; j < w; j++)
        k += (int)q[j] * pow(e, j);
    return k;
}
