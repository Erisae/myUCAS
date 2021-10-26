/*已知 k 阶斐波那契序列的定义为

 

f(0)=0, f(1)=0, ... , f(k-2)=0, f(k-1)=1;

f(n)=f(n-1)+f(n-2)+...+f(n-k); n=k, k+1, ...

 

试编写求 k 阶斐波那契序列的(第 m 项)值的函数算法-----根据实例可知表示的是下标

其中k>1
*/
#include <stdio.h>
int main()
{
    int k, m;
    int res, i, j;
    int a[1000];
    scanf("%d %d", &k, &m);
    if (m < k - 1)
        res = 0;
    else if (m == k - 1)
        res = 1;
    else
    {
        for (i = 0; i <= m; i++)
            a[i] = 0;
        for (i = 0; i < k - 1; i++)
            a[i] = 0;
        a[i++] = 1;
        for (; i <= m; i++)
        {
            for (j = 1; j <= k; j++)
            {
                a[i] += a[i - j];
            }
        }
        res = a[m];
    }
    printf("%d", res);
}
