/*
已知记录序列a[1..n]中的关键字各不相同，可按如下所述实现计数排序：另设数组c[1..n]，
对每个记录a[i]，统计序列中关键字比它小的记录个数存于c[i]，
则c[i]=0的记录必为关键字最小的记录，
然后依c[i]值的大小对a中记录进行重新排列，试编写算法实现上述排序方法。15
*/
#include <stdio.h>
#define size 15

void GetIn(int ar[size])
{
    int i=0;
    int c;
    while(1)
    {
        scanf("%d",&ar[i]);
        i++;
        if((c=getchar())=='\n')
        break;
    }
}

void GetLower(int A[size],int C[size])//比A[i]的元素个数小的个数
{
    int i,j;
    for(i=0;i<15;i++)//用于比较的那个
    {
        C[i]=0;
        for(j=0;j<15;j++)
        {
            if(A[j]<A[i])
            C[i]++;
        }
    }
}

void Arrange(int A[size],int C[size])
{
    int B[size];//中间
    int i;
    for(i=0;i<15;i++)
    {
        B[C[i]]=A[i];
    }
    for(i=0;i<15;i++)
    {
        A[i]=B[i];
    }
}

void PrintNum(int A[size])
{
    int i;
    for(i=0;i<15;i++)
    {
        if(i==0)
        printf("%d",A[i]);
        else
        printf(" %d",A[i]);
    }
}

int main()
{
    int a[size],c[size];
    GetIn(a);
    GetLower(a,c);
    Arrange(a,c);
    PrintNum(a);
    return 0;
}
//0 25 60 91 73 10 27 1 19 55 62 45 37 59 43