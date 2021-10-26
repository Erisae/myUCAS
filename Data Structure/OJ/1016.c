/*Description:
实现strCompare(S, T)函数,即,若S>T,则返回值1,若S=T则返回值0,若S<T则返回值-1
Input:
字符串S,T,以空格分隔*/
#include <stdio.h>
int strCompare(char *,char *);
int main()
{
    //输入st
    char s[100] = {'\0'}, t[100] = {'\0'};
    int result;
    scanf("%s", s);
    scanf("%s", t);
    result=strCompare(s,t);
    printf("%d",result);
    return 0;
}

int strCompare(char *str1,char *str2)//比较字符串大小的函数
{
    while(*str1==*str2 && *str1!='\0' && *str2!='\0')//掠过相同的部分
    {
        str1++;
        str2++;
    }
    //分情况讨论 还有没有
    if(*str1=='\0' && *str2!='\0')
    return -1;
    else if(*str1!='\0' && *str2=='\0')
    return 1;
    else if(*str1=='\0' && *str2=='\0')
    return 0;
    else if (*str1 > *str2)
    return 1;
    else if(*str1 < *str2)
    return -1;
    else if (*str1==*str2)
    return 0;
}