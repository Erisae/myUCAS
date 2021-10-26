#include <stdio.h>
#include <ctype.h>

#define MAX 1000

void inputs(char[], char[]);
int compares(char[], char[]);

int main()
{
    char va[MAX], vb[MAX];
    inputs(va, vb);
    printf("%d", compares(va, vb));

    return 0;
}
void inputs(char a1[], char b1[])
{
    int i = 0, c;
    while (i < MAX)
    {
        if (isalpha(c = getchar()))
        {
            a1[i] = c;
            i++;
        }
        else if (c == ';')
        {
            a1[i] = '\0';
            break;
        }
    }
    i = 0;
    while (i < MAX)
    {
        if (isalpha(c = getchar()))
        {
            b1[i] = c;
            i++;
        }
        else if (c == '\n')
        {
            b1[i] = '\0';
            break;
        }
    }
}
int compares(char a1[], char b1[])
{
    int i = 0;
    while ((a1[i] == b1[i]) && a1[i] != '\0' && b1[i] != '\0')
        i++;
    if (a1[i] =='\0' &&  b1[i] == '\0')
        return 0;
    else if ((a1[i] > b1[i]) || b1[i] == '\0')
        return 2;
    else
        return 1;
}
