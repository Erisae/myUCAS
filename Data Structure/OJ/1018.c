/*
Description:
假设以定长顺序存储结构表示串,试设计一个算法,求串s中出现的第一个最长重复子串及其位置 
Input:
字符串s    
Output:
最长重复子串及其位置,用空格分隔,如果没有重复子串则输出-1
*/
//可能的子串长度为2~n/2
//枚举
//先按照长度——2，依次12 23 在后面匹配(条件是后面的长度大于等于匹配长度) 匹配到一个了就记录位置和串
//增加长度3,123 234…… 直到长度为n/2

#include <stdio.h>
#include <string.h>

typedef struct mem //用来存匹配成功的数据
{
    char string[100];
    int index;
} mem;
int KMP(char *, char *);
int next[100] = {'\0'};
void Next(char *, int *);
int main()
{
    char str[200] = {'\0'};
    char *str1, *str2;          //指示位置的指针
    int str1_index;             //和上面那个如影随形
    char str_ref[100] = {'\0'}; //用来匹配的串
    mem str_get;
    int len_ref, len;
    int i, j;
    scanf("%s", str);
    len = strlen(str);
    str1 = str;
    

    //开始
    for (len_ref = 2; len_ref <= len / 2; len_ref++) //子串匹配长度
    {
        for (str1_index = 0,str1=str; str1_index + len_ref <= len - 1; str1_index++, str1++) //取子串开始的位置
        {
            str2 = str1; //存之后的位置
            for (i = str1_index, j = 0; i < str1_index + len_ref; i++, j++, str2++)
                str_ref[j] = str[i]; //获得匹配的子串 i是下一个位置
            if (KMP(str2, str_ref) != -1)
            {
                //先把str_get.string清空
                for (i = 0; i < 100; i++)
                    str_get.string[i] = '\0';
                //再将str_ref给他
                for (i = 0; str_ref[i] != '\0'; i++)
                    str_get.string[i] = str_ref[i];
                str_get.index = str1_index;
                break; //相同长度最前面匹配成功的读 跳出循环（长度增加）
            }
            else //把str_ref清空
            {
                for (i = 0; i < 100; i++)
                    str_ref[i] = '\0';
            }
        }
        //进入下一个循环之前不用清空str_ref 因为ref会变长 把之前的覆盖了
    }
    if(str_get.string[0]=='\0')
    printf("-1");
    else
    {
        printf("%s ",str_get.string);
        printf("%d",str_get.index);
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