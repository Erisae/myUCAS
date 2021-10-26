#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "head.h"

void BoardSetting();
void DisplayBoard();
void BeginPlay(int q[]);
void GetAddress();
void GetReady();
void win(int); //��Ӯ1 ��Ӯ2
void Judge();
void genboard();
int transfer(int);

int pattern[2] = {0, 0}; // 0λ�����洢��Ϸģʽ��1λ�����洢��Ϸ��ɫ

int x;		   //�е�ַ(����)
int y;		   //�е�ַ�����£�
int count = 1; //���� ˭����

int Board[15][15];		   //15�� 15��   0~14
static int iboard[15][15]; //AI�õ� ֻ��0,1��2
int oboard[15][15];		   //old ����ǰ������

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
	Board[14][0] = 21;  //���½� 21
	Board[14][14] = 11; //���½� 11
	Board[0][0] = 1;	//���Ͻ� 1  ******���Ͻ��ǿ�ʼ
	Board[0][14] = 31;  //���Ͻ� 31

	for (i = 1; i < 14; i++)
		Board[14][i] = 41; //�±� 41
	for (i = 1; i < 14; i++)
		Board[0][i] = 51; //�ϱ� 51
	for (i = 1; i < 14; i++)
		Board[i][0] = 61; //��� 61
	for (i = 1; i < 14; i++)
		Board[i][14] = 71; //�ұ� 71

	for (i = 1; i < 14; i++)
	{
		for (j = 1; j < 14; j++)
			Board[i][j] = 81; //�м� 81
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
				printf("�� ");
				break;
			case 11:
				printf("��");
				break;
			case 21:
				printf("�� ");
				break;
			case 31:
				printf("��");
				break;
			case 41:
				printf("�� ");
				break;
			case 51:
				printf("�� ");
				break;
			case 61:
				printf("�� ");
				break;
			case 71:
				printf("��");
				break;
			case 81:
				printf("�� ");
				break;
			case 3:
				printf("��");
				break;
			case 4:
				printf("��");
				break;
			case 13:
				printf("��");
				break;
			case 14:
				printf("��");
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
	printf("��ѡ����Ϸģʽ��\n  1 ���˶�ս			2 �˻���ս\n");
	scanf("%d", &pattern[0]);
	if (pattern[0] == 2)
	{
		printf("��ѡ���ɫ��\n 1 ִ��			2 ִ��\n"); /*����ʾ����*/
		scanf("%d", &pattern[1]);
	}
}
// void GetAddress()
// {
// 	char address[3];
// 	int i = 0; //ѭ������
// 	int j, k;  //jΪ�� kΪ��
// 	printf("��������һ��λ��");
// S1:
// 	scanf("%s", address);
// 	if (isdigit(address[i]))
// 	{
// 		for (j = 0; isdigit(address[i]); i++) //����û��ֱ�ӿ����õĿ⺯��
// 			j = j * 10 + address[i] - '0';	//�ظ��Ŀ�����ôѭ��ʹ�ã�

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

// 	if (!(j >= 1 && j <= 15 && k >= 0 && k <= 14) || Board[j][k] % 10 == 3 || Board[j][k] % 10 == 4) //��Χ�� û����
// 	{
// 		printf("��������ȷ��λ��");
// 		goto S1;
// 	}

// 	x = 15 - j;
// 	y = k;
// }


void GetAddress()
{
	char address[3];
	int i = 0; //ѭ������
	int j, k;  //jΪ�� kΪ��
	do
	{
		printf("��������һ��λ��");
		scanf("%s", address);
		if (isdigit(address[i]))
		{
			for (j = 0; isdigit(address[i]); i++) //����û��ֱ�ӿ����õĿ⺯��
				j = j * 10 + address[i] - '0';	//�ظ��Ŀ�����ôѭ��ʹ�ã�

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
void BeginPlay(int q[2]) //q 1 ���� q 2 �˻�
{
	int a1, b1;
	int a2, b2; //1 2��Ӧִ�ڻ���ִ�� ab��Ӧ����
	a1 = b1 = a2 = b2 = 0;
	Spoint *h;
	if (q[0] == 1) //����ģʽ
	{
		while (x >= 0 && x <= 14 && y >= 0 && y <= 14) //��֤�ڷֳ���Ӯ֮ǰһֱѭ��
		{
			GetAddress();
			if (count == 1) //�ڵ�һ��
			{
				Board[x][y] = 13;
				a1 = x;
				b1 = y;
			}
			else if (count == 2) //�׵�һ��
			{
				Board[x][y] = 14;
				Board[a1][b1] = 3;
				a2 = x;
				b2 = y;
			}
			else //���ǵ�һ��
			{
				if (count % 2 == 1) //��ִ�ڵ���
				{
					Board[x][y] = 13;
					Board[a2][b2] = 4;
					a1 = x;
					b1 = y;
				}
				else if (count % 2 == 0) //��ִ�׵���
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
	else if (q[0] == 2) //�˻�ģʽ
	{
		while (x >= 0 && x <= 14 && y >= 0 && y <= 14)
		{
			if (q[1] == 1) //�˺�
			{
				if (count % 2 == 1) //����
				{
					GetAddress();
					if (count == 1) //�ڵ�һ��
					{
						Board[x][y] = 13;
						a1 = x;
						b1 = y;
					}
					else if (count % 2 == 1)
					{ //��ִ�ڵ���

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

				else if (count % 2 == 0) //���ߡ���������
				{
					h = search(iboard, White, AI_SEARCH_DEPTH);
					x = h->x;
					y = h->y;
					free(h);

					if (count == 2) //�׵�һ��
					{
						Board[x][y] = 14;
						Board[a1][b1] = 3;
						a2 = x;
						b2 = y;
					}
					else if (count % 2 == 0) //��֮��
					{
						Board[x][y] = 14;
						Board[a1][b1] = 3;
						a2 = x;
						b2 = y;
					}
					system("cls");
					system("color F0");
					DisplayBoard();
					printf("��������Ϊ��%c %d \n", (char)(y + 'A'), (int)(15 - x));
					genboard();
					Judge();
					count++;
				}
			}
		else if (q[1] == 2) //�˰�
		{
			if (count % 2 == 0) //��ɫ�� ��Ϊ����
			{
				GetAddress();
				if (count == 2) //�׵�һ��
				{
					Board[x][y] = 14;
					Board[a1][b1] = 3;
					a2 = x;
					b2 = y;
				}
				else //��ִ�׵���
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

			else //��������
			{
				if (count == 1) //�ڵ�һ��
				{
					x = 7;
					y = 7;
					Board[x][y] = 13;
					a1 = x;
					b1 = y;
				}
				else //��ִ�ڵ���
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
				printf("��������Ϊ��%c %d \n", (char)(y + 'A'), (int)(15 - x));
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
	if (count % 2 == 0) //���������
	{
		if (types[FIVE])
			win(2);
	}
	else if (count % 2 == 1) //���������
	{
		if (types[FIVE])
			win(1);
		// TODO: fix bug
		else if (types[LONG] || (types[FOUR] + types[SFOUR] >= 2) || types[THREE] >= 2) //�������֡�44���֡�33����) //
		{
			printf("�������ڽ���");
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
		printf("dogfall(ƽ��)");
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
			iboard[i][j] = transfer(Board[i][j]); //ps:board��ȫ�ֱ���
	}
}

int checkboard (int board[15][15])    //�������п��򷵻�1 ȫ������0
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
