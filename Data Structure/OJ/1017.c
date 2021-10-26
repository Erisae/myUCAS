/*Description:
编写算法,实现串的基本操作Replace(S, T, V),使用V替换S中出现的所有与T相等的不重叠的子串.
若S中不存在T,则返回空字符串
Input:
字符串S,T,V,以空格分隔
Output:
替换之后的字符串*/

#include <stdio.h>
#include <string.h>
void Next(char *, int *);
int replace(char *, char *, char *, char *);
int KMP(char *, char *);
int next[100] = {'\0'};
int main()
{
    char s[100] = {'\0'}, t[100] = {'\0'}, v[100] = {'\0'}, a[100] = {'\0'}; //用a来存现在的
    int flag;                                                                //为1说明没有匹配
    scanf("%s", s);
    scanf("%s", t);
    scanf("%s", v);
    Next(t, next); //获得next数组
    flag = replace(s, t, v, a);
    if (flag == 1)
        printf("%c", '\n');
    else
        printf("%s", a);
    return 0;
}
//获得a的函数
int replace(char *str1, char *str2, char *str3, char *get)
{
    int ind, indp;
    char *str11 = str1;
    int temp[100];
    int i = 0, j, k, is, q; //temp的位置
    for (q = 0; q < 100; q++)
        temp[q] = -1;
    while ((ind = KMP(str11, str2)) != -1 && *str11 != '\0') //退出条件KMP没有匹配
    {
        temp[i++] = ind - 1;
        str11 += ind + strlen(str2) - 1;
    }
    i = 0;
    j = 0;
    k = 0;
    is = 0;
    if (temp[0] == -1)
        return 1;
    else
    {
        while ((temp[is] != -1)) //发现问题 \0和0无法区分  换成-1
        {
            if (i == 0)
            {
                ind = temp[is];
                indp = 0; //indp记录上一次之后的
            }
            else
            {
                indp = ind + strlen(str2);
                ind += strlen(str2) + temp[i];
            }
            for (i = indp; i < ind; i++)
                get[j++] = str1[i];
            while (str3[k] != '\0')
                get[j++] = str3[k++];

            is++;
        }
        indp = ind + strlen(str2);
        for (i = indp; str1[i] != '\0'; i++)
            get[j++] = str1[i];
    }
    return 0;
}

//next函数
void Next(char *str2, int *nexts)
{
    int i = 1, j = 0;
    nexts[i] = 0;
    while (i < strlen(str2))
    {
        if (j == 0 || str2[i - 1] == str2[j - 1])
        {
            i++;
            j++;
            nexts[i] = j;
        }
        else
        {
            j = nexts[j];
        }
    }
}

//kmp算法
int KMP(char *str1, char *str2) //返回匹配的第一个地址
{
    int i = 1, j = 1;
    while (i <= strlen(str1) && j <= strlen(str2))
    {

        if (j == 0 || str1[i - 1] == str2[j - 1])
        {
            i++;
            j++;
        }
        else
        {
            j = next[j];
        }
    }
    if (j > strlen(str2))
    {
        return i - (int)strlen(str2);
    }
    return -1;
}