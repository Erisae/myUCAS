/*Description:
编写算法,求得所有包含在串s中而不包含在串t中的字符(s中重复的字符只选一个)构成的新串r,以及r中每个字符在s中第一次出现的位置.
Input:
字符串s和字符串t,通过空格分割
Output:
新串r和r中每个字符第一次出现在s中的位置,通过空格分割,如果不存在字符串r则输出"-1"*/

#include <stdio.h>

int main()
{
    char s[100] = {'\0'}, t[100] = {'\0'}, r[100] = {'\0'}; //s t为初始的字符串 r为输出的
    int a[100]={'\0'};                                             //a存放index
    int ind_s, ind_t, ind_r=0, ind_rs;                    //分别是在str串中的坐标
    char tempt;
    int flag1=0, flag2 = 0;
    scanf("%s", s);
    scanf("%s", t);
    for (ind_s = 0; s[ind_s] != '\0'; ind_s++)
    {
        tempt = s[ind_s];
        //看该元素是不是s特有的
        for (ind_t = 0; t[ind_t] != '\0'; ind_t++)
        {
            if (tempt == t[ind_t])
            {
                flag1 = 1; //flag1为1说明该元素是st共有的
                break;
            }
        }
        //看该元素是不是已经放在r里面了
        if (flag1 != 1)
        {
            for (ind_rs = 0; r[ind_rs] != 0; ind_rs++)
            {
                if (tempt == r[ind_rs])
                {
                    flag2 = 1; //flag2为1说明该元素已经放在r里面了
                    break;
                }
            }
        }
        if (flag1 == 0 && flag2 == 0)
        {
            r[ind_r] = tempt;
            a[ind_r++] = ind_s;
        }
        flag1=0;flag2=0;
    }

    //打印
    ind_rs=0;
    if(ind_r==0)
    printf("-1");
    else
    {
        while(r[ind_rs]!='\0')
        {
            printf("%c",r[ind_rs]);
            ind_rs++;
        } 
        printf(" ");
        ind_rs=0;
        while(ind_rs<ind_r)
        {
            printf("%d",a[ind_rs]);
            ind_rs++;
        }

    }
   


    return 0;
}