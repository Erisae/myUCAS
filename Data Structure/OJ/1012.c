/*Description:
试写一个判别表达式中开,闭括号是否配对出现的算法.   比如:'(a+b(c-d))'是配对出现的,而'(d-e))'不是配对出现的.
Input:
表达式,如:'a+b(c-d)'*/
#include <stdio.h>
int main()
{
    int c;
    int x=0;
    while((c=getchar())!='\n')
    {
        if(c=='(')
        x++;
        else if(c==')')
        x--;
        if(x<0)
        {
            printf("0");
            return 0;
        }
    }
    if (x==0)
    printf("1");
    else 
    {
        printf("0");
    }
    return 0;
    
}