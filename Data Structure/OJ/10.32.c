//荷兰国旗问题
#include <stdio.h>
#define max 100

void swap(int a[],int i,int j)
{
    int temp;
    temp=a[i];
    a[i]=a[j];
    a[j]=temp;
}

int main()
{
    int s[max] = {'\0'};
    int c, i = 0;
    while (1)
    {
        scanf("%d", &c);
        s[i++] = c + 1;
        if ((c = getchar()) == '\n')
            break;
    }
    int low, high, ptr; //low指示的是0，high指示的是2
    low = 0;
    high = i - 1;
    ptr = 0;
    while (ptr <= high)
    {
        if (s[ptr] == 1) //最前面
        {
            swap(s, ptr, low);
            low++;
            ptr++;//换过来的可能是1，2，3
        }
        else if(s[ptr]==3)
        {
            swap(s,ptr,high);
            high--;
        }
        else ptr++;
    }
    int j;
    for(j=0;j<=i-1;j++)
    {
        if(j==0)
        printf("%d",s[j]-1);
        else
        printf(",%d",s[j]-1);
    }
    return 0;
}
//0,1,0,0,0,1,1,2,2,1,0,2,1,0