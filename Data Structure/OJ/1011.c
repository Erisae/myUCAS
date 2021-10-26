#include <stdio.h>
#include <ctype.h>
int main()
{
    char a[100],b[100];
    int c,k,i=0,j=0;
    while((c=getchar())!='&')
    {
    a[i]=c;
    i++;
    }
    a[i]='\0';
    while((c=getchar())!='@')
    {
        b[j]=c;
        j++;
    }
    b[j]='\0';
    for(k=0;k<(i<j?i:j);k++)
    {
        if(a[k]!=b[j-1-k])
        break;
    }
    if(k==i && k==j)
    printf("1");
    else
    printf("0");
    return 0;
}