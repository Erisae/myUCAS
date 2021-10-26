#include <stdio.h>
void range1(int*,int*,int*);
void swap1(int*,int*);


int main()
{
    int x, y, z;
    while (scanf("%d %d %d", &x, &y, &z) != EOF)
    {
        range1(&x, &y, &z);
        printf("%d %d %d", x, y, z);
    }
    return 0;
}

void range1(int *a, int *b, int *c)
{
    if (*b > *a)
        swap1(a, b);
    if (*c > *a )
    {
        swap1(a,c);
        swap1(b,c);
    }
    else if (*c > *b)
        swap1(b, c);
}

void swap1(int *m, int *n)
{
    int k;
    k = *m;
    *m = *n;
    *n = k;
}