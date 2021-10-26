//通常 & 正确 ->逆波兰
/*
一、 将中缀表达式转换成后缀表达式算法：
　　1、从左至右扫描一中缀表达式。
　　2、若读取的是操作数，则判断该操作数的类型，并将该操作数存入操作数堆栈
　　3、若读取的是运算符
　　　　(1) 该运算符为左括号"("，则直接存入运算符堆栈。
　　　　(2) 该运算符为右括号")"，则输出运算符堆栈中的运算符到操作数堆栈，直到遇到左括号为止。
　　　　(3) 该运算符为非括号运算符：
　　　　　　(a) 若运算符堆栈栈顶的运算符为括号（只可能是左括号），则直接存入运算符堆栈。
　　　　　　(b) 若比运算符堆栈栈顶的运算符优先级高，则直接存入运算符堆栈。
　　　　　　(c) 若比运算符堆栈栈顶的运算符优先级低或相等，则不断输出栈顶运算符到操作数堆栈，直到栈顶没有运算符的优先级大于或者等于当前预算符（即栈顶存在运算符的话，优先级一定是小于当前运算符），最后将当前运算符压入运算符堆栈。
　　4、当表达式读取完成后运算符堆栈中尚有运算符时，则依序取出运算符到操作数堆栈，直到运算符堆栈为空。
A+B*(C-D)-E*F
*/
#include <stdio.h>

int main()
{
    char num_buf[1000];
    char sig_buf[1000];
    char c;
    int i;
    int r; //标志
    int p_num, p_sig;
    p_num = 0;
    p_sig = 0;

    for (i = 0; i < 1000; i++)
    {
        num_buf[i] = '\0';
        sig_buf[i] = '\0';
    }
    r = 0;
    while ((c = getchar()) != '\n')
    {
        switch (c)
        {
        case 'A' ... 'Z': //√
        case 'a'...'z':
        {
            num_buf[p_num++] = c;
            break;
        }
        case '(': //√
        {
            sig_buf[p_sig++] = c;
            break;
        }
        case ')': //√
        {
            p_sig--; //p_sig不可能是0
            while (sig_buf[p_sig] != '(')
            {
                num_buf[p_num++] = sig_buf[p_sig];
                sig_buf[p_sig] = '\0';
                p_sig--;
            }
            sig_buf[p_sig] = '\0';
            break;
        }
        case '+':
        case '-':
        {
            if (p_sig > 0)
            {
                p_sig--;
                while (sig_buf[p_sig] != '(' && p_sig >= 0)
                {
                    num_buf[p_num++] = sig_buf[p_sig];
                    sig_buf[p_sig] = '\0';
                    p_sig--;
                }
                sig_buf[++p_sig] = c;
                p_sig++;
            }
            else
                sig_buf[p_sig++] = c;
            break;
        }
        case '*':
        case '/':
        {
            if (p_sig > 0)
            {
                p_sig--;
                while ((sig_buf[p_sig] == '*' || sig_buf[p_sig] == '/') && p_sig>=0)
                {
                    num_buf[p_num++] = sig_buf[p_sig];
                    sig_buf[p_sig] = '\0';
                    p_sig--;
                }
                sig_buf[++p_sig] = c;
                p_sig++;
            }
            else
                sig_buf[p_sig] = c;
            break;
        }
        default:
            break;
        }
    }
    if(p_sig>0)
    p_sig--;
    while (p_sig >= 0)
    {
        num_buf[p_num++] = sig_buf[p_sig];
        sig_buf[p_sig] = '\0';
        p_sig--;
    }

    for (i = 0; i < p_num; i++)
        printf("%c", num_buf[i]);
    return 0;
}
