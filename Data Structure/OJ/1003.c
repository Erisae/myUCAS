//计算i!*2^i(i=0,1,2,...,n-1)的值并存入a[arrsize](arrsize=300)的各个分量中，
//假设计算机允许最大值为MAXINT(在你自己的程序中定义，MAXINT=65535)，
//则当n>arrsize或者i!*2^i>MAXINT时，出错处理。输出结果
#include <stdio.h>
#include "math.h" //调用pow(m,n) (int)
#define MAXINT 65535
#define arrsize 300
int cal(int i); //运算
int fat(int x); //阶乘

int main()
{
    int n, j, b;
    int a[arrsize];
    scanf("%d", &n);
        for (j = 0; j < n; j++)
        {
            b=cal(j);
            if (b > MAXINT)
            {
                break;
            }
            else
                a[j] =b;
        }
        printf("%d",a[j-1]);

    return 0;
}

int fat(int x)
{
    int y = 1;
    for (int i = 1; i <= x; i++)
        y *= i;
    return y;
}

int cal(int i)
{
    int res;
    res = (int)fat(i) * pow(2, i);
    return res;
}
