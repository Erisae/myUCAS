#include <stdio.h>
int main()
{
    int c;
    int i = 0, j = 0;  //注意要分开，不可以写 i，j
    int w;
    char a[1000];
    while ((c = getchar()) != '@' && i < 1000)  //注意内存错误：有可能是用char类型，但是输入中文输入法
    {
        a[i] = c;
        i++;
    }
    a[i] = '\0';
    while (a[i - 1 - j] == a[j])
        j++;
    if (j == i)
        printf("1");
    else
        printf("0");
    return 0;
}