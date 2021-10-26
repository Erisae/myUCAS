//回文
#include <stdio.h>
int main()
{
    char a[1000];
    int p1,p2;//分别指向前后的标志（指针）
    int c;
    int i=0;
    while((c=getchar())!='@')
    {
        a[i]=c;
        i++;
    }
    a[i]='\0';
    p1=0;p2=--i;
    while(p2>=p1)
    {
        if(a[p2]==a[p1])
        {
            a[p2]='\0';
            a[p1]='\0';
            p1++;
            p2--;
        }
        else 
        {
            printf("0");
            return 0;
        }
    }
    printf("1");
    return 0;
}