#include <stdio.h>
#include <stdlib.h>
typedef struct size
{
    int row;    //行
    int column; //列
} size;
size ini_matrix(int a[100][100]);
int get_bridge(int m[100][100], int *a, size s);
int get_min_j(int i1, int mm[100][100], size ss, int *);
int get_max(int j1, int mm[100][100], size ss);
int cmpfunc(const void *a, const void *b);
void printh(int *a, int n);

int main()
{
    int matrix[100][100] = {'\0'};
    int bridge[100];
    int num;
    size matrix_size;
    matrix_size = ini_matrix(matrix);
    num = get_bridge(matrix, bridge, matrix_size);
    qsort(bridge, num, sizeof(int), cmpfunc);
    printh(bridge, num);
    return 0;
}
size ini_matrix(int a[100][100])
{
    int c;
    int i = 0, j = 0; //i行 j列
    size temp;
    while (scanf("%d", &c) == 1)
    {
        a[i][j] = c;
        if ((c = getchar()) == ',')
            j++;
        else if (c == ';')
        {
            i++;
            j = 0;
        }
        else if (c == '\n')
            break;
    }
    temp.row = i + 1;
    temp.column = j + 1;
    return temp;
}

int get_bridge(int m[100][100], int *a, size s)
{
    //算法：找第i行中的最小值（不用村），存其坐标j 与相应j的最大值作比较，
    int i, max_temp, num_j;
    int j[100] = {'\0'};
    i = 0;
    int c;
    int index = 0;
    while (i < s.row)
    {
        num_j = get_min_j(i, m, s, j);
        for (c = 0; c < num_j; c++)
        {
            max_temp = get_max(j[c], m, s);
            if (max_temp == m[i][j[c]])
            {
                a[index] = m[i][j[c]];
                index++;
            }
        }
        i++;
    }
    return index;
}

int get_min_j(int i1, int mm[100][100], size ss, int *jj)
{
    int j, temp_j;
    int temp_num;
    int index = 0;
    for (j = 0, temp_num = mm[i1][0], temp_j = 0; j < ss.column; j++)
    {
        if (mm[i1][j] < temp_num)
        {
            temp_num = mm[i1][j];
            temp_j = j;
        }
    }
    //get return and jj
    jj[index] = temp_j;
    for (j = temp_j+1, index = 1; j < ss.column; j++)
    {
        if (mm[i1][j] == mm[i1][temp_j])
        {
            jj[index] = j;
            index++;
        }
        return index;
    }
}

int get_max(int j1, int mm[100][100], size ss)
{
    int i;
    int temp_num;
    for (i = 0, temp_num = mm[i][j1]; i < ss.row; i++)
    {
        if (mm[i][j1] > temp_num)
            temp_num = mm[i][j1];
    }
    return temp_num;
}
int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

void printh(int *a, int n)
{
    int i = 0;
    if (n == 0)
        printf("null");
    else
    {
        while (i < n - 1)
        {
            printf("%d,", a[i]);
            i++;
        }
        printf("%d", a[i]);
    }
}