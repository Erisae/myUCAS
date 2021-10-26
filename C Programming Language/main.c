#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "head.h"

void BoardSetting();
void DisplayBoard();
void BeginPlay(int q[]);
void GetAddress();
void GetReady();
void win(int); //黑赢1 白赢2
void Judge();
void genboard();
int transfer(int);

int pattern[2] = {0, 0}; // 0位用来存储游戏模式；1位用来存储游戏角色

int x;		   //行地址(当下)
int y;		   //列地址（当下）
int count = 1; //计数 谁在下

int Board[15][15];		   //15行 15列   0~14
static int iboard[15][15]; //AI用的 只有0,1，2
int oboard[15][15];		   //old 落子前的棋盘

int main()
{
	system("color F0");
	BoardSetting();
	DisplayBoard();
	GetReady();
	BeginPlay(pattern);
	return 0;
}

void BoardSetting()
{
	int i, j;
	Board[14][0] = 21;  //左下角 21
	Board[14][14] = 11; //右下角 11
	Board[0][0] = 1;	//左上角 1  ******左上角是开始
	Board[0][14] = 31;  //右上角 31

	for (i = 1; i < 14; i++)
		Board[14][i] = 41; //下边 41
	for (i = 1; i < 14; i++)
		Board[0][i] = 51; //上边 51
	for (i = 1; i < 14; i++)
		Board[i][0] = 61; //左边 61
	for (i = 1; i < 14; i++)
		Board[i][14] = 71; //右边 71

	for (i = 1; i < 14; i++)
	{
		for (j = 1; j < 14; j++)
			Board[i][j] = 81; //中间 81
	}
}

void DisplayBoard()
{
	int i, j;
	for (i = 0; i <= 14; i++)
	{
		for (j = 0; j <= 14; j++)
		{
			if (j == 0)
				printf("%2d", 15 - i);
			switch (Board[i][j])
			{
			case 1:
				printf("┏ ");
				break;
			case 11:
				printf("┛");
				break;
			case 21:
				printf("┗ ");
				break;
			case 31:
				printf("┓");
				break;
			case 41:
				printf("┻ ");
				break;
			case 51:
				printf("┳ ");
				break;
			case 61:
				printf("┣ ");
				break;
			case 71:
				printf("┫");
				break;
			case 81:
				printf("╋ ");
				break;
			case 3:
				printf("●");
				break;
			case 4:
				printf("◎");
				break;
			case 13:
				printf("▲");
				break;
			case 14:
				printf("△");
				break;
			}

			if (j == 14)
				printf("\n");
		}
	}
	printf("  A B C D E F G H I J K L M N O \n");
}

void GetReady()
{
	printf("\nWelcome to gobang\n\n");
	printf("请选择游戏模式：\n  1 人人对战			2 人机对战\n");
	scanf("%d", &pattern[0]);
	if (pattern[0] == 2)
	{
		printf("请选择角色：\n 1 执黑			2 执白\n"); /*加显示规则*/
		scanf("%d", &pattern[1]);
	}
}
// void GetAddress()
// {
// 	char address[3];
// 	int i = 0; //循环控制
// 	int j, k;  //j为行 k为列
// 	printf("请输入下一步位置");
// S1:
// 	scanf("%s", address);
// 	if (isdigit(address[i]))
// 	{
// 		for (j = 0; isdigit(address[i]); i++) //找有没有直接可以用的库函数
// 			j = j * 10 + address[i] - '0';	//重复的可以怎么循环使用？

// 		if (address[i] >= 'a' && address[i] <= 'o')
// 			k = address[i] - 'a';
// 		else if (address[i] >= 'A' && address[i] <= 'O')
// 			k = address[i] - 'A';
// 	}
// 	else if (isalpha(address[i]))
// 	{
// 		if (address[i] >= 'a' && address[i] <= 'o')
// 			k = address[i++] - 'a';
// 		else if (address[i] >= 'A' && address[i] <= 'O')
// 			k = address[i++] - 'A';

// 		for (j = 0; isdigit(address[i]); i++)
// 			j = j * 10 + address[i] - '0';
// 	}

// 	if (!(j >= 1 && j <= 15 && k >= 0 && k <= 14) || Board[j][k] % 10 == 3 || Board[j][k] % 10 == 4) //范围内 没下子
// 	{
// 		printf("请输入正确的位置");
// 		goto S1;
// 	}

// 	x = 15 - j;
// 	y = k;
// }


void GetAddress()
{
	char address[3];
	int i = 0; //循环控制
	int j, k;  //j为行 k为列
	do
	{
		printf("请输入下一步位置");
		scanf("%s", address);
		if (isdigit(address[i]))
		{
			for (j = 0; isdigit(address[i]); i++) //找有没有直接可以用的库函数
				j = j * 10 + address[i] - '0';	//重复的可以怎么循环使用？

			if (address[i] >= 'a' && address[i] <= 'o')
				k = address[i] - 'a';
			else if (address[i] >= 'A' && address[i] <= 'O')
				k = address[i] - 'A';
		}
		else if (isalpha(address[i]))
		{
			if (address[i] >= 'a' && address[i] <= 'o')
				k = address[i++] - 'a';
			else if (address[i] >= 'A' && address[i] <= 'O')
				k = address[i++] - 'A';

			for (j = 0; isdigit(address[i]); i++)
				j = j * 10 + address[i] - '0';
		}
	} while (
		!(j >= 1 && j <= 15 && k >= 0 && k <= 14) || Board[15 - j][k] % 10 == 3 || Board[15 - j][k] % 10 == 4);

	x = 15 - j;
	y = k;
}
void BeginPlay(int q[2]) //q 1 人人 q 2 人机
{
	int a1, b1;
	int a2, b2; //1 2对应执黑还是执白 ab对应行列
	a1 = b1 = a2 = b2 = 0;
	Spoint *h;
	if (q[0] == 1) //人人模式
	{
		while (x >= 0 && x <= 14 && y >= 0 && y <= 14) //保证在分出输赢之前一直循环
		{
			GetAddress();
			if (count == 1) //黑第一次
			{
				Board[x][y] = 13;
				a1 = x;
				b1 = y;
			}
			else if (count == 2) //白第一次
			{
				Board[x][y] = 14;
				Board[a1][b1] = 3;
				a2 = x;
				b2 = y;
			}
			else //不是第一次
			{
				if (count % 2 == 1) //即执黑的下
				{
					Board[x][y] = 13;
					Board[a2][b2] = 4;
					a1 = x;
					b1 = y;
				}
				else if (count % 2 == 0) //即执白的下
				{
					Board[x][y] = 14;
					Board[a1][b1] = 3;
					a2 = x;
					b2 = y;
				}
			}
			system("cls");
			system("color F0");
			DisplayBoard();
			genboard();
			Judge();
			count++;
		}
	}
	else if (q[0] == 2) //人机模式
	{
		while (x >= 0 && x <= 14 && y >= 0 && y <= 14)
		{
			if (q[1] == 1) //人黑
			{
				if (count % 2 == 1) //黑走
				{
					GetAddress();
					if (count == 1) //黑第一次
					{
						Board[x][y] = 13;
						a1 = x;
						b1 = y;
					}
					else if (count % 2 == 1)
					{ //即执黑的下

						Board[x][y] = 13;
						Board[a2][b2] = 4;
						a1 = x;
						b1 = y;
					}
					system("cls");
					system("color F0");
					DisplayBoard();
					genboard();
					Judge();
					count++;
				}

				else if (count % 2 == 0) //白走、即机器走
				{
					h = search(iboard, White, AI_SEARCH_DEPTH);
					x = h->x;
					y = h->y;
					free(h);

					if (count == 2) //白第一次
					{
						Board[x][y] = 14;
						Board[a1][b1] = 3;
						a2 = x;
						b2 = y;
					}
					else if (count % 2 == 0) //白之后
					{
						Board[x][y] = 14;
						Board[a1][b1] = 3;
						a2 = x;
						b2 = y;
					}
					system("cls");
					system("color F0");
					DisplayBoard();
					printf("机器落子为：%c %d \n", (char)(y + 'A'), (int)(15 - x));
					genboard();
					Judge();
					count++;
				}
			}
		else if (q[1] == 2) //人白
		{
			if (count % 2 == 0) //白色走 即为人走
			{
				GetAddress();
				if (count == 2) //白第一次
				{
					Board[x][y] = 14;
					Board[a1][b1] = 3;
					a2 = x;
					b2 = y;
				}
				else //即执白的下
				{
					Board[x][y] = 14;
					Board[a1][b1] = 3;
					a2 = x;
					b2 = y;
				}
				system("cls");
				system("color F0");
				DisplayBoard();
				genboard();
				Judge();
				count++;
			}

			else //机器下棋
			{
				if (count == 1) //黑第一次
				{
					x = 7;
					y = 7;
					Board[x][y] = 13;
					a1 = x;
					b1 = y;
				}
				else //即执黑的下
				{
					h = search(iboard, Black, AI_SEARCH_DEPTH);
					x = h->x;
					y = h->y;
					free(h);
					Board[x][y] = 13;
					Board[a2][b2] = 4;
					a1 = x;
					b1 = y;
				}
				system("cls");
				system("color F0");
				DisplayBoard();
				printf("机器落子为：%c %d \n", (char)(y + 'A'), (int)(15 - x));
				genboard();
				Judge();
				count++;
			}
		}
	}
}
system("pause");
}

void Judge()
{
	int types[11];
	Point pp;
	pp.x = x, pp.y = y;
	alltypes(iboard, &pp, 2 - count % 2, types);
	if (count % 2 == 0) //刚下完白棋
	{
		if (types[FIVE])
			win(2);
	}
	else if (count % 2 == 1) //刚下完黑棋
	{
		if (types[FIVE])
			win(1);
		// TODO: fix bug
		else if (types[LONG] || (types[FOUR] + types[SFOUR] >= 2) || types[THREE] >= 2) //长连禁手、44禁手、33禁手) //
		{
			printf("黑子下在禁点");
			win(2);
		}
	}
	else if (checkboard(iboard)==0)
	{
		win(3);
	}
}

void win(int r)
{
	if (r == 1)
	{
		printf("black wins");
		x = 15;
	}
	else if (r == 2)
	{
		printf("white wins");
		x = 15;
	}
	else if (r==3)
	{
		printf("dogfall(平局)");
		x=15;
	}
}

int transfer(int g)
{
	if (g % 10 == 1)
		return 0;
	else if (g % 10 == 3)
		return 1;
	else if (g % 10 == 4)
		return 2;
	else
		return -1;
}

void genboard()
{
	int i, j;
	for (i = 0; i < 15; i++)
	{
		for (j = 0; j < 15; j++)
			iboard[i][j] = transfer(Board[i][j]); //ps:board是全局变量
	}
}

int checkboard (int board[15][15])    //棋盘上有空则返回1 全满返回0
{
	int i,j;
	for(i=0;i<15;i++)
	{
		for(j=0;j<15;j++)
		{
			if(board[i][j]==0)
			return 1;  
		}
	}
	return 0;
}
