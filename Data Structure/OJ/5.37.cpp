#pragma warning(disable : 4996)
//删除原子项x但不删除产生的空表
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Atomtype char
typedef enum
{
	ATOM,
	LIST
} Elemtag; //ATOM == 0, LIST == 1
typedef struct GLNode
{
	Elemtag tag;	   //tag区分是表结点还是原子结点
	union {			   //原子结点和表结点的联合部分
		Atomtype atom; //atom 是原子结点的值域
		struct
		{
			struct GLNode *hp, *tp;
		} ptr; //ptr.hp和ptr.tp分别指向表头和表尾
	};
} * GList; //广义表类型

int SubString(char *sub, char *str, int start, int len)
{
	int count = 0;
	for (; count < len; count++)
		*(sub + count) = *(str + start - 1 + count);
	*(sub + count) = '\0'; //末尾开一位作为终结符
	return 0;
}
int sever(char *&str, char *&hstr) //将非空串str分割成两部分，hsub为最外层','分割的第一个子串，str为之后的子串
{								   //没有分配额外空间，str与hstr都是指针
	int n, i, k;
	Atomtype ch;
	n = strlen(str);
	i = 0;
	k = 0; //k记录尚未配对左括号数
	do
	{
		ch = *(str + i);
		i++;
		if (ch == '(')
			k++;
		else if (ch == ')')
			k--;
	} while (i < n && (ch != ',' || k != 0));
	if (i < n)
	{						   //字符串未读完
		*(str + i - 1) = '\0'; //分割字符串
		hstr = str;
		str = str + i;
	}
	else
	{
		hstr = str;
		str = NULL; //说明已经是字表最后一项，不能再分割
	}
	return 0;
}

int CreateGList(GList &L, char *S) //由字符串S创建广义表L,S末尾必须有终结符'\0'
{
	GLNode *p, *q;
	int len;
	if (0 == strcmp(S, "()"))
		L = NULL; //创建空表
	else
	{
		if (!(L = (GList)malloc(sizeof(GLNode))))
			exit(0); //创建一个新结点
		if (strlen(S) == 1)
		{
			L->tag = ATOM;
			L->atom = *(S); //创建单原子广义表
		}
		else
		{ //递归处理子广义表
			L->tag = LIST;
			p = L;
			len = strlen(S);
			char *sub = (char *)calloc(len - 1, sizeof(char)); //留一位存'\0'
			char *hsub;
			SubString(sub, S, 2, len - 2); //脱括号
			while (1)
			{
				sever(sub, hsub); //分离出表头串hsub
				CreateGList(p->ptr.hp, hsub);
				q = p;
				if (sub)
				{ //表尾不空，在字表后面上添加并列的字表
					if (!(p = (GLNode *)malloc(sizeof(GLNode))))
						exit(0);
					p->tag = LIST;
					q->ptr.tp = p;
				}
				else
					break;
			}
			q->ptr.tp = NULL; //每一层最后字表的tp设为NULL
			free(sub);		  //把每一层为脱括号新开的空间释放
		}
	}
	return 0;
}

int Print_GList(GList L) //递归打印广义表
{
	if (L == NULL)
		printf("()");
	else
	{
		if (L->tag == ATOM)
		{ //原子类型
			printf("%c", L->atom);
		}
		else
		{ //子广义表，递归打印
			printf("(");
			GLNode *ptr = L;
			while (ptr)
			{
				Print_GList(ptr->ptr.hp);
				if (ptr->ptr.tp)
				{ //当有后继结点时
					printf(",");
				}
				ptr = ptr->ptr.tp;
			}
			printf(")");
		}
	}
	return 0;
}

int Del_Atom_x(GList &L, Atomtype x) //递归删除广义表L中的原子项x
{									 //删除原子项x但不删除产生的空表
	GList temp;
	if (L == NULL || L->tag == ATOM)
		return 0; //空表
	else
	{
		if (L->tag == LIST && L->ptr.hp->tag == ATOM)
		{ //原子类型
			if (L->ptr.hp->atom == x)
			{
				temp = L->ptr.tp;
				free(L);
				L = temp;
			}
			else
			{
				GLNode *ptr = L;
				while (ptr && ptr->tag != ATOM)
				{
					Del_Atom_x(ptr->ptr.tp, x);
					ptr = ptr->ptr.tp;
				}
			}
		}
		else
		{
			while(L && L->tag!=ATOM)
			L=L->ptr.hp;
		}
	}
	return 0;
}

int main()
{
	char x;
	printf("Please input a valid GList format string S, whose length < 1000:\n");
	char str[1000] = "";
	scanf("%s", str);
	GList L;
	CreateGList(L, str);
	printf("This is the GList(string format):\n");
	Print_GList(L);
	printf("\n");
	printf("Please enter the element x you want to delete:\n");
	getchar(); //清除上一个scanf留下的字符
	x = getchar();
	Del_Atom_x(L, x);
	printf("This is the GList after delete x:\n");
	Print_GList(L);
	printf("\n");
	return 0;
}

/*
//子广义表
			GLNode *ptr = L;
			while (ptr && ptr->tag!=ATOM  ) {
				Del_Atom_x(ptr->ptr.hp, x);
				ptr = ptr->ptr.tp;
			}*/