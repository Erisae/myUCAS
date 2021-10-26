//压栈出栈
#include <stdio.h>
int main()
{
    char a[1000];
    int c;
    int i=0;
    while((c=getchar())!='&')
    {
        a[i]=c;
        i++;
    }
    a[i]='\0';
    i--;
    while((c=getchar())!='@' && i>=0)
    {
        if(a[i]==c)
        {
            a[i]='\0';
            i--;
        }
        else i--;
    }
    if(a[0]=='\0')
    printf("1");
    else 
    printf("0");

    return 0;

}