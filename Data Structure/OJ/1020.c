#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
void prints(int dim[6], int data, int n, int c);
void remake(int dim[6], int n, int le);
int getnum(char *str, int index, int *);
void printh(char s[1000], int l, int d);
int last(int d[6],int dim);
int main()
{
    int dim;
    int len;
    char str[1000] = {'\0'};
    scanf("%d", &dim);
    getchar();
    scanf("%d", &len); //对应每一个维度的长度
    scanf("%s", str);
    getchar();
    printh(str, len, dim);
    return 0;
}

void printh(char s[1000], int l, int d)
{
    int i, j;
    int num;
    int ls;
    for (i = 0; s[i] != '\0'; i++)
        ; //找到最后的位置
    int xdim[6] = {0, l - 1, l - 1, l - 1, l - 1, l - 1};
    int count = 0;
    for (j = i - 1; j >= 0; j--)
    {
        if (isdigit(s[j]))
        {
            num = getnum(s, j,&ls); //获得在那的那个数据 可以是负的
            j-=ls-1;
            count++;
            if (num != 0)
                prints(xdim, num, d, count);
            remake(xdim, d, l);
        }
    }
}

int getnum(char *str, int index,int * sl)
{
    char s1[100] = {'\0'};
    char temp[100] = {'\0'};
    int j = 0, i;
    while (isdigit(str[index]) || str[index] == '-')
    {
        s1[j] = str[index];
        j++;
        index--;
    }
    strcpy(temp, s1);
    for (j = strlen(s1) - 1, i = 0; j >= 0; j--, i++)
        s1[i] = temp[j];
    *sl=strlen(s1);
    return atoi(s1);
}

void remake(int dim[6], int n, int le) //减1
{
    int i, j; //后面的影响前面的
    for (i = n; i >= 1; i--)
    {
        if (dim[i] != 0)
        {
            dim[i] = dim[i] - 1;
            for (j = i + 1; j <= n; j++)
                dim[j] = le - 1;
            break;
        }
    }
}

void prints(int dim[6], int data, int n, int c)
{
    int i;
    if (c == 1) //第一个数
    {
        if (data == -1)
            printf("-");
        else if (data < 0)
            printf("%d", data);
        else if (data == 1)
            ;
        else if (data > 0)
            printf("%d", data);
    }
    else
    {
        if (last(dim, n) && data == 1) //是最后一个数为1
            printf("+1");
        else
        {
            if (data == -1)
                printf("-");
            else if (data == 1)
                printf("+");
            else if (data < 0)
                printf("%d", data);
            else if (data > 0)
                printf("+%d", data);
        }
    } //系数

    for (i = 1; i <= n; i++)
    {
        if (dim[i] == 1)
            printf("x%d", i);
        else if (dim[i] > 0)
            printf("x%dE%d", i, dim[i]);
    } //指数
}

int last(int d[6],int dim)
{
    int i;
    for(i=dim;i>=1;i--)
    {
        if(d[i]!=0)
        return 0;
    }
    return 1;
}