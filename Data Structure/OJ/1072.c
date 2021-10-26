//递归函数
#include <stdio.h>
int g(int,int);
int main()
{
    int m,n,x;
    scanf("%d,%d",&m,&n);
    x=g(m,n);
    printf("%d",x);
    return 0;
}

int g(int a,int b)
{
    if (a==0)
    return 0;
    return g(a-1,2*b)+b;
}