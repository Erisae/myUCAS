//()
#include <stdio.h>
int main()
{
    int c;
    char a[1000];
    int i = -1;
    int j;
    for (j=0;j<1000;j++)
    {
        a[j]='\0';
    }
    while ((c = getchar()) != '\n')
    {
        if (c == '(' && i >= -1)
        {
            i++;
            a[i] = '(';
        }
        else if (c == ')' && i >= 0)
        {
            a[i] = '\0';
            i--;
        }
        else if (c == ')' && i < 0)
        {
            printf("0");
            return 0;
        }
    }
    if(a[0]=='\0')
    printf("1");
    else
    printf("0");
    return 0;
}