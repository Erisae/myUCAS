//消除递归 意思就是用循环来代替递归
#include <stdio.h>
int main()
{
    int n;
    int res;
    scanf("%d",&n);
    //这个函数本质上就是 n*n/2*(n/2)/2*^*1
    res=1;    
    while(n!=0)
    {
        res=res*n;
        n=n/2;
    }
    printf("%d",res);

    return 0;
}
