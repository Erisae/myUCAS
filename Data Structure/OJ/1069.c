/*
Description:
4.10 编写对串求逆的递推算法。注：字符串的长度不会超过200，且可能包含空格。
Input:
一个字符串 s
Output:
一个字符串，表示对 s 求逆的结果
*/
#include <stdio.h>
#include <string.h>
void getreverse(char *, char *);
int main()
{
    char str1[200] = {'\0'};
    char *str2,*str11;
    int c,i=0;
    while((c=getchar())!='\n')
    str1[i++]=c;
    //对字符串求逆 递归
    str11=str1;
    str2=strlen(str1)+str1-1;
    getreverse(str1, str2); //传指针
    printf("%s",str11);
    return 0;
}

void getreverse(char *stra, char *strb)
{
    char temp;
    if(strb<=stra)
    return ;
    else
    {
        temp=*stra;
        *stra=*strb;
        *strb=temp;
        getreverse(++stra,--strb);
    }
}