//用三元组存储稀疏矩阵
#include <stdio.h>
typedef struct size
{
    int row;
    int col;
} size;
typedef struct item
{
    int num;
    int row;
    int col;
} item;
int getin(size *s, item *b);
void printh(size s, item *b, int n);
int main()
{
    size matrix_size;
    item matrix_b[100];
    int num;
    num = getin(&matrix_size, matrix_b); //matric_b s是矩阵的稀疏存储
    printh(matrix_size, matrix_b, num);
    return 0;
}

int getin(size *s, item *b)
{
    //size
    int c;
    scanf("%d", &c);
    s->row = c;
    getchar();
    scanf("%d", &c);
    s->col = c;
    getchar();
    getchar();
    getchar();
    //get content
    int i, j;
    int index = 0;
    for (i = 0; i < s->row; i++)
    {
        for (j = 0; j < s->col; j++)
        {
            scanf("%d", &c);
            if (c != 0)
            {
                b[index].num = c;
                b[index].col = j;
                b[index].row = i;
                index++;
            }
            getchar();
        }
        getchar();
        getchar();
    }
    return index;
}

void printh(size s, item *b, int n)
{
    //输出所有不为0的
    int i, j;
    int index;
    if (n == 0)
        printf("[]");
    else
    {
        printf("[");
        for (i = 0; i < n - 1; i++)
            printf("%d,", b[i].num);
        printf("%d]", b[i].num);
    }
    //输出中间的
    printf(";[");
    for (i = 0, index = 0; i < s.row; i++)
    {
        printf("[");
        for (j = 0; j < s.col; j++)
        {
            if (i == b[index].row && j == b[index].col && n!=0)
            {
                if (j == s.col - 1)
                    printf("%d", 1);
                else
                    printf("%d,", 1);
                index++;
            }
            else
            {
                if (j == s.col - 1)
                    printf("%d", 0);
                else
                    printf("%d,", 0);
            }
        }
        printf("]");
        if (i == s.row - 1)
            printf("]");
        else
            printf(";");
    }
}

//there might be other expections under extreme circumstances