/*假设有 A，B，C，D，E 五个高等院校进行田径对抗赛，各院校的单项成绩均已存入计算机，并构成一 张表，表中每一行的形式为
项目名称 性别 校名 得分
编写算法，处理上述表格，以统计各院校的男、女总分和团体总分，并输出。
其中，表格的记录数量N<=100
三种项目，分别为X,Y,Z
性别分为F和M
学校分为A,B,C,D,E
input
X M A 10
X F A 10
X M B 20
Y F C 15
output
A M 10
A F 10
A 20
B M 20
B 20
C F 15
C 15*/
#include <stdio.h>
typedef struct
{
    char item;
    char gender;
    char school;
    int score;
} row;

typedef struct
{
    int boyscore;
    int girlscore;
    int allscore;
} schools;

int main()
{
    row a[100];
    schools b[5];
    int c;
    int i, j, k;
    for (i = 0; i < 100; i++)
    {
        a[i].item = 0;
        a[i].gender = 0;
        a[i].school = 0;
        a[i].score = 0;
    }
    i = 0;
    while (scanf("%c %c %c %d", &a[i].item, &a[i].gender, &a[i].school, &a[i].score) == 4)
    {
        if ((c = getchar()) == EOF)
            break;
        else
            i++;
    }

    for (k = 0; k <= 4; k++)
    {
        b[k].boyscore = 0;
        b[k].girlscore = 0;
        b[k].allscore = 0;
    }
    for (j = 0; j <= i-1; j++)
    {
        switch (a[j].school)
        {
        case 'A':
        {
            if (a[j].gender == 'F')
                b[0].girlscore += a[j].score;
            else
                b[0].boyscore += a[j].score;
            b[0].allscore += a[j].score;
            break;
        }
        case 'B':
        {
            if (a[j].gender == 'F')
                b[1].girlscore += a[j].score;
            else
                b[1].boyscore += a[j].score;
            b[1].allscore += a[j].score;
            break;
        }
        case 'C':
        {
            if (a[j].gender == 'F')
                b[2].girlscore += a[j].score;
            else
                b[2].boyscore += a[j].score;
            b[2].allscore += a[j].score;
            break;
        }
        case 'D':
        {
            if (a[j].gender == 'F')
                b[3].girlscore += a[j].score;
            else
                b[3].boyscore += a[j].score;
            b[3].allscore += a[j].score;
            break;
        }
        case 'E':
        {
            if (a[j].gender == 'F')
                b[4].girlscore += a[j].score;
            else
                b[4].boyscore += a[j].score;
            b[4].allscore += a[j].score;
            break;
        }
        default:
            break;
        }
    }
    for (k = 0; k <= 4; k++)
    {
        if (b[k].boyscore != 0)
            printf("%c M %d\n", 'A' + k, b[k].boyscore);
        if (b[k].girlscore != 0)
            printf("%c F %d\n", 'A' + k, b[k].girlscore);
        if (b[k].allscore != 0)
            printf("%c %d\n", 'A' + k, b[k].allscore);
    }

    return 0;
}
