#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "head.h"
#define SCORE_FIVE 1000000

Point global_bestmove;
int tboard[17][17];

int MAPSCORE[15][15] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
						{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
						{0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0},
						{0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0},
						{0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0},
						{0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0},
						{0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0},
						{0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0},
						{0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0},
						{0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0},
						{0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0},
						{0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0},
						{0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0},
						{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
						{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

// void deleteSubtree(TreeNode* node) {

//     if (node -> left != NULL){
//       deleteSubtree(node -> left);
//       node -> left = NULL;
//     }

//     if (node -> right != NULL){
//       deleteSubtree(node -> right);
//       node -> right = NULL;
//     }

//     free(node); 
// }

// int alphaBetaPruning(TreeNode* node, int alpha, int beta, bool max){

// 	if(node->left == NULL){
// 		return node->value;	
// 	}
	
// 	if(max){// if max
// 		int max1 = INT_MIN;
// 		int value = alphaBetaPruning(node->left, alpha, beta, false);
// 		max1 = max(value, max1);
// 		alpha = max(alpha, max1);
// 		if(beta <= alpha){
// 			deleteSubtree(node->right);  //pruning only in left
// 			node->right = NULL;
// 			return max1;
// 		} 
		
// 		value = alphaBetaPruning(node->right, alpha, beta, false);
// 		max1 = max(value, max1);
// 		return max1;
// 	}
// 	else {//if min
// 		int min1 = INT_MAX;
// 		int value = alphaBetaPruning(node->left, alpha, beta, true);
// 		min1 = min(value, min1);
// 		beta = min(beta, min1);
// 		if(beta <= alpha){
// 			deleteSubtree(node->right);  //pruning only in right
// 			node->right = NULL;
// 			return min1;
// 		}
		
// 		value = alphaBetaPruning(node->right, alpha, beta, true);
// 		min1 = min(value, min1);
// 		return min1;
// 	}
// }

Spoint * search (int board[15][15],int player,int depth)
{
    Spoint moves[10];
    int xboard[15][15];
    Spoint *search_p = (Spoint *)malloc(sizeof(Spoint));
    for(int i=0; i<15; i++)
	{
		for(int j=0; j<15; j++)
		xboard[i][j]=board[i][j];   //????????????board???xboard?????? ???????????????????????????
	}
    for(int i=0;i<10;i++)
    	moves[i].x=moves[i].y=moves[i].score=-1;
    GenMove(player, xboard, moves);
    search_p->x=moves[0].x;
    search_p->y=moves[0].y;
    return search_p;

}
int HasNeighbor(int board[15][15], Point *p, int radius) //??????
{
	int i, j;
	int start_x = max(p->x - radius, 0), end_x = min(p->x + radius, 14);
	int start_y = max(p->y - radius, 0), end_y = min(p->y + radius, 14);
	if (board[p->x][p->y] == 0) //?????????????????????????????????
	{
		for (i = start_x; i < end_x + 1 && i < 15; i++)
		{
			for (j = start_y; j < end_y + 1 && j < 15; j++)
			{
				if (board[i][j] != 0)
					return 1;
			}
		}
	}
	return 0;
}

//qsort??????
int cmpspoint(const void *_a, const void *_b) //??????????????????
{
	return (*(Spoint *)_a).score > (*(Spoint *)_b).score ? -1 : 1;
}

//????????????????????????????????????????????????10?????? int moves[10]
void GenMove(int player, int board[15][15], Spoint moves[10]) //?????????player????????????????????????
{
	Spoint allmoves[225];
	int k=0;
    int p=0;
	int i, j;
	Point o;
	for (k = 0; k <= 225; k++)
	{
		allmoves[k].x = -1;
		allmoves[k].y = -1;
		allmoves[k].score = -1;
	}
	k = 0;
	for (i = 0; i <= 14; i++)
	{
		for (j = 0; j <= 14; j++)
		{
			o.x = i;
			o.y = j;
			if (HasNeighbor(board, &o, 1))
			{
				allmoves[k].x = i;
				allmoves[k].y = j;
				allmoves[k].score = EvaluateChess(board, &o, player); //????????????
				k++;
			}
		}
	} //???????????????????????????allmoves?????????
	qsort(allmoves, k, sizeof(allmoves[0]), cmpspoint);
	while (p <= (k > 9 ? 9 : k - 1))
	{
		moves[p].x = allmoves[p].x;
		moves[p].y = allmoves[p].y;
		moves[p].score = allmoves[p].score;
		p++;
	} //??????????????? ????????????moves??????10??? ????????????????????????
}
void gentboard(int board[15][15], int t1board[17][17])
{
	int i, j;
	//??????tboard ????????????
	for (j = 0; j <= 16; j++)
	{
		t1board[0][j] = 3;
		t1board[16][j] = 3;
	}
	for (i = 0; i <= 16; i++)
	{
		t1board[i][0] = 3;
		t1board[i][16] = 3;
	}
	for (i = 1; i <= 15; i++)
	{
		for (j = 1; j <= 15; j++)
			t1board[i][j] = board[i - 1][j - 1];
	}
}

//0???  1???  2???  3??????
int mygetline(Point *p, int a, char x[17]) //a???4????????????0 ?????? 1?????? 2???????????? 3????????????
{
	int q; //????????????
	int e = 0;
	switch (a)
	{
	case 0: //?????? ????????????
		for (q = 0; q <= 16; q++)
			x[q] = tboard[p->x + 1][q] + '0';
		e = p->y + 1;
		x[q] = '\0';
		break;
	case 1: //?????? ????????????
		for (q = 0; q <= 16; q++)
			x[q] = tboard[q][p->y + 1] + '0';
		e = p->x + 1;
		x[q] = '\0';
		break;
	case 2: //???????????? ????????????
		if (p->x > p->y)
		{
			for (q = 0; q <= 16 - p->x + p->y; q++)
				x[q] = tboard[p->x - p->y + q][q] + '0';
			e = p->y + 1;
		}
		else if (p->x <= p->y)
		{
			for (q = 0; q <= 16 - p->y + p->x; q++)
				x[q] = tboard[q][p->y - p->x + q] + '0';
			e = p->x + 1;
		}
		x[q] = '\0';
		break;
	case 3: //???????????? ????????????
		if (p->y + p->x <= 14)
		{
			for (q = 0; q <= p->x + p->y + 2; q++)
				x[q] = tboard[p->x + p->y + 2 - q][q] + '0';
			e = p->y + 1;
		}
		else if (p->x + p->y > 14)
		{
			for (q = 0; q <= 30 - p->x - p->y; q++)
				x[q] = tboard[16 - q][p->x + p->y - 14 + q] + '0';
			e = 14 - p->x;
		}
		x[q] = '\0';
		break;
	}
	return e;
}

int match(char s[], char s1[], int e) //e????????? ???s[]????????????
{
	int lenth_s1 = strlen(s1);
	int i = 0, j = 0;
	int count = 0;
	while (s[i + lenth_s1] != '\0' && i + lenth_s1 < 17)
	{
		while (s[j + i] == s1[j] && j <= lenth_s1 - 1)
			j++;
		if ((j == lenth_s1) && (e >= i) && (e <= i + lenth_s1))
			count++; //????????????
		else		 //???????????? ??????????????????
		{
			j = 0;
			while (s[j + i] == s1[lenth_s1 - 1 - j] && j <= lenth_s1 - 1)
				j++;
			if ((j == lenth_s1) && (e >= i) && (e <= i + lenth_s1))
				count++; //??????????????????
		}
		j = 0;
		i++;
	}
	return count;
} //???????????????1 ??????????????????0

// int match(char s[], char s1[], int e) //e????????? ???s[]????????????
// {
// 	int lenth_s1 = strlen(s1);
// 	int i = 0, j = 0;
// 	int count = 0;
// 	while (s[i + lenth_s1] != '\0' && i + lenth_s1 < 17)
// 	{
// 		while (s[j + i] == s1[j] && j <= lenth_s1 - 1)
// 			j++;
// 		if ((j == lenth_s1) && (e >= i) && (e <= i + lenth_s1))
// 			count++; //????????????
// 		else
// 			j = 0;
// 		while (s[j + i] == s1[lenth_s1 - 1 - j] && j <= lenth_s1 - 1)
// 			j++;
// 		if ((j == lenth_s1) && (e >= i) && (e <= i + lenth_s1))
// 			count++;
// 		else
// 			j = 0;
// 		i++;
// 	}
// 	return count;
// } //???????????????1 ??????????????????0

void chesstype(char s[], int e, int a, int player, int Type[4][11]) //ps:count?????? ?????????????????????   //?????? ????????????
{
	if (player == Black) //???????????????
	{
		Type[a][LONG] = match(s, "111111", e) || match(s, "1111111", e) || match(s, "11111111", e) || match(s, "111111111", e) || match(s, "1111111111", e) || match(s, "11111111111", e) || match(s, "111111111111", e) + match(s, "1111111111111", e) || match(s, "11111111111111", e) || match(s, "111111111111111", e);
		Type[a][FIVE] = match(s, "11111", e);																																																																										 //???5
		Type[a][FOUR] = match(s, "011110", e);																																																																										 //??????????????????????????????5
		Type[a][SFOUR] = match(s, "10111", e) + match(s, "11011", e) + match(s, "311110", e) + match(s, "211110", e);																																																								 //???4?????????????????????
		Type[a][THREE] = match(s, "001110", e) / 2 + match(s, "010110", e);																																																																			 //??????????????????????????????
		Type[a][DTHREE] = match(s, "211100", e) + match(s, "311100", e) + match(s, "211010", e) + match(s, "311010", e) + match(s, "210110", e) + match(s, "310110", e) + match(s, "11001", e) + match(s, "10101", e) + match(s, "2011102", e) + match(s, "3011102", e) + match(s, "3011103", e);													 //??????????????????????????????
		Type[a][TWO] = match(s, "001100", e) + match(s, "010100", e) / 2 + match(s, "010010", e);																																																													 //???2????????????????????????2
		Type[a][DTWO] = match(s, "000112", e) + match(s, "000113", e) + match(s, "0010112", e) + match(s, "001013", e) + match(s, "010012", e) + match(s, "010013", e) + match(s, "10001", e) + match(s, "2010102", e) + match(s, "3010102", e) + match(s, "3010103", e) + match(s, "2011002", e) + match(s, "2011003", e) + match(s, "3011002", e); //???2???????????????3???2
		Type[a][SIFOUR] = match(s, "211112", e) + match(s, "211113", e) + match(s, "311113", e);																																																													 //???4
		Type[a][SITHREE] = match(s, "21112", e) + match(s, "21113", e) + match(s, "31113", e);																																																														 //???3
		Type[a][SITWO] = match(s, "2112", e) + match(s, "2113", e) + match(s, "3113", e);																																																															 //??????
	}
	else if (player == White)
	{
		Type[a][LONG] = match(s, "222222", e) || match(s, "2222222", e) || match(s, "22222222", e) || match(s, "222222222", e) || match(s, "2222222222", e) || match(s, "22222222222", e) || match(s, "222222222222", e) || match(s, "2222222222222", e) || match(s, "22222222222222", e) || match(s, "222222222222222", e);
		Type[a][FIVE] = match(s, "22222", e);																																																																										 //???5
		Type[a][FOUR] = match(s, "022220", e);																																																																										 //??????????????????????????????5
		Type[a][SFOUR] = match(s, "20222", e) + match(s, "22022", e) + match(s, "322220", e) + match(s, "122220", e);																																																								 //???2?????????????????????
		Type[a][THREE] = match(s, "002220", e) / 2 + match(s, "020220", e);																																																																			 //??????????????????????????????
		Type[a][DTHREE] = match(s, "122200", e) + match(s, "322200", e) + match(s, "122020", e) + match(s, "322020", e) + match(s, "120220", e) + match(s, "320220", e) + match(s, "22002", e) + match(s, "20202", e) + match(s, "1022201", e) + match(s, "3022201", e) + match(s, "3022203", e);													 //??????????????????????????????
		Type[a][TWO] = match(s, "002200", e) + match(s, "020200", e) / 2 + match(s, "020020", e);																																																													 //???1????????????????????????1
		Type[a][DTWO] = match(s, "000221", e) + match(s, "000223", e) + match(s, "0020221", e) + match(s, "002023", e) + match(s, "020021", e) + match(s, "020023", e) + match(s, "20002", e) + match(s, "1020201", e) + match(s, "3020201", e) + match(s, "3020203", e) + match(s, "1022001", e) + match(s, "1022003", e) + match(s, "3022001", e); //???1???????????????3???1
		Type[a][SIFOUR] = match(s, "122221", e) + match(s, "122223", e) + match(s, "322223", e);																																																													 //???2
		Type[a][SITHREE] = match(s, "12221", e) + match(s, "12223", e) + match(s, "32223", e);																																																														 //???3
		Type[a][SITWO] = match(s, "1221", e) + match(s, "1223", e) + match(s, "3223", e);																																																															 //??????
	}
}

void alltypes(int board[15][15], Point *p, int player, int type[11]) //?????????????????? ???????????????????????????????????????type??????
{
	char x0[17], x1[17], x2[17], x3[17];
	int e0, e1, e2, e3;
	int Type[4][11];
	int i, j;
	//?????????
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 11; j++)
			Type[i][j] = 0;
	}
	for (i = 0; i < 11; i++)
		type[i] = 0;
	gentboard(board, tboard);
	e0 = mygetline(p, 0, x0); //?????? p?????????Point?????????
	e1 = mygetline(p, 1, x1);
	e2 = mygetline(p, 2, x2);
	e3 = mygetline(p, 3, x3);
	chesstype(x0, e0, 0, player, Type);
	chesstype(x1, e1, 1, player, Type);
	chesstype(x2, e2, 2, player, Type); //fix bug: 2.3
	chesstype(x3, e3, 3, player, Type);
	for (i = 0; i < 11; i++)
		type[i] = Type[0][i] + Type[1][i] + Type[2][i] + Type[3][i];
}

int marking(int board[15][15], Point *p, int player)
{
	int type[11] = {0};
	int score = 0;
	alltypes(board, p, player, type);
	if (player == White)			  //??????????????????
	{								  //?????? ???????????????
		if (type[FIVE] || type[LONG]) //ps???????????????????????????????????????
			score = 1000000;			  //??????
		else if (type[FOUR] || (type[SFOUR] >= 2) || (type[SFOUR] && type[THREE]))
			score = 100000; //???4?????????4??????4???3
		else if (type[THREE] >= 2)
			score = 50000; //??????3
		else if (type[THREE] && type[DTHREE])
			score = 10000; //???3???3
		else if (type[SFOUR])
			score = 5000; //???4
		else if (type[THREE])
			score = 2000; //???3
		else if (type[TWO] >= 2)
			score = 1000; //??????2
		else if (type[DTHREE])
			score = 500; //???3
		else if (type[TWO] && type[DTWO])
			score = 100; //???2???2
		else if (type[TWO])
			score = 50; //???2
		else if (type[DTWO])
			score = 30; //???2
		else if (type[SIFOUR])
			score = -50; //???4
		else if (type[SITHREE])
			score = -50; //???3
		else if (type[SITWO])
			score = -50; //???2
	}
	else //?????? ?????????
	{

		if (type[FIVE])		//ps???????????????????????????????????????
			score = 1000000; //??????
		else if ((type[LONG] || (type[FOUR] + type[SFOUR] >= 2) || type[THREE] >= 2))
			score = -1000000; //??????
		else if (type[FOUR] || (type[SFOUR] && type[THREE]))
			score = 100000; //???4??????4???3
		else if (type[THREE] && type[DTHREE])
			score = 10000; //???3???3
		else if (type[SFOUR])
			score = 5000; //???4
		else if (type[THREE])
			score = 2000; //???3
		else if (type[TWO] >= 2)
			score = 1000; //??????2
		else if (type[DTHREE])
			score = 500; //???3
		else if (type[TWO] && type[DTWO])
			score = 100; //???2???2
		else if (type[TWO])
			score = 50; //???2
		else if (type[DTWO])
			score = 30; //???2
		else if (type[SIFOUR])
			score = -50; //???4
		else if (type[SITHREE])
			score = -50; //???3
		else if (type[SITWO])
			score = -50; //???2
	}
	return score + MAPSCORE[p->x][p->y];
}

int EvaluateChess(int board[15][15], Point *p, int player) //???????????????????????????????????? ?????????
{
	int chessscore;
	int a, b;
	board[p->x][p->y] = player;
	a = marking(board, p, player);
	board[p->x][p->y] = 3 - player;
	b = marking(board, p, 3 - player);
	chessscore = 7*a + 5*b;
	board[p->x][p->y]=0;
	return chessscore;
}

int matchs(char s[], char s1[]) //????????????s????????????s1 ???????????????
{
	int lenth_s1 = strlen(s1);
	int i = 0, j = 0;
	int count = 0;
	while (s[i + lenth_s1] != '\0' && i + lenth_s1 < 17)
	{
		if (s1[0] == s[i])
		{
			while (s[j + i] == s1[j] && j <= lenth_s1 - 1)
				j++;
			if (j == lenth_s1)
				count++; //????????????
		}
		else //????????????
		{
			if (s1[lenth_s1 - 1] == s[i]) //??????
			{
				while (s[j + i] == s1[lenth_s1 - 1 - j] && j <= lenth_s1 - 1)
					j++;
				if (j == lenth_s1)
					count++; //??????????????????
			}
		}
		j = 0;
		i++;
	}
	return count;
}

void gensboard(int t1board[17][17], char s1board[4][33][17]) //???t1boaard???s1board ????????????char???
{
	int i, j;
	//??????
	for (i = 0; i < 17; i++)
	{
		for (j = 0; j < 17; j++)
			s1board[0][i][j] = t1board[i][j] + '0';
	}
	s1board[0][17][0] = '\0';
	//??????
	for (i = 0; i < 17; i++)
	{
		for (j = 0; j < 17; j++)
			s1board[1][i][j] = t1board[j][i] + '0';
	}
	s1board[1][17][0] = '\0';
	//2,4??????  ??????????????????
	for (i = 0; i < 17; i++)
	{
		for (j = 0; 16 - i + j <= 16 && j <= 16; j++)
			s1board[2][i][j] = t1board[j][16 - i + j] + '0';
		s1board[2][i][j] = '\0';
	}
	for (i = 17; i < 33; i++)
	{
		for (j = 0; i - 16 + j <= 16 && j <= 16; j++)
			s1board[2][i][j] = t1board[i - 16 + j][j] + '0';
		s1board[2][i][j] = '\0';
	}

	//1,3??????  ??????????????????
	for (i = 0; i < 17; i++)
	{
		for (j = 0; i - j >= 0 && j <= 16; j++)
			s1board[3][i][j] = t1board[j][i - j] + '0';
		s1board[3][i][j] = '\0';
	}
	for (i = 17; i < 33; i++)
	{
		for (j = 0; i + j - 16 <= 16 && j <= 16; j++)
			s1board[3][i][j] = t1board[i + j - 16][16 - j] + '0';
		s1board[3][i][j] = '\0';
	}
}

int evaluatea(int board[15][15], int player) //????????????????????????
{
	int a, i, j;
	int Type[4][11];	//4????????? 20?????????
	int tboard[17][17]; //???????????????
	char sboard[4][33][17];
	int score;
	int boardscore;
	gentboard(board, tboard);
	gensboard(tboard, sboard);
	int type[11];

	//?????????
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 11; j++)
		{
			Type[i][j] = 0;
		}
	}
	for (a = 0; a < 4; a++)
	{
		for (i = 0; sboard[a][i][0] != '\0'; i++)
		{
			if (player == Black) //?????????
			{
				Type[a][LONG] = matchs(sboard[a][i], "111111") || matchs(sboard[a][i], "1111111") || matchs(sboard[a][i], "11111111") || matchs(sboard[a][i], "111111111") || matchs(sboard[a][i], "1111111111") || matchs(sboard[a][i], "11111111111") || matchs(sboard[a][i], "111111111111") || matchs(sboard[a][i], "1111111111111") || matchs(sboard[a][i], "11111111111111") || matchs(sboard[a][i], "111111111111111");
				Type[a][FIVE] = matchs(sboard[a][i], "11111");																																																																																																					  //???5
				Type[a][FOUR] = matchs(sboard[a][i], "011110");																																																																																																					  //??????????????????????????????5
				Type[a][SFOUR] = matchs(sboard[a][i], "10111") + matchs(sboard[a][i], "11011") + matchs(sboard[a][i], "311110") + matchs(sboard[a][i], "211110");																																																																												  //???4?????????????????????
				Type[a][THREE] = matchs(sboard[a][i], "001110") / 2 + matchs(sboard[a][i], "010110");																																																																																											  //??????????????????????????????
				Type[a][DTHREE] = matchs(sboard[a][i], "211100") + matchs(sboard[a][i], "311100") + matchs(sboard[a][i], "211010") + matchs(sboard[a][i], "311010") + matchs(sboard[a][i], "210110") + matchs(sboard[a][i], "310110") + matchs(sboard[a][i], "11001") + matchs(sboard[a][i], "10101") + matchs(sboard[a][i], "2011102") + matchs(sboard[a][i], "3011102") + matchs(sboard[a][i], "3011103");																	  //??????????????????????????????
				Type[a][TWO] = matchs(sboard[a][i], "001100") + matchs(sboard[a][i], "010100") / 2 + matchs(sboard[a][i], "010010");																																																																																			  //???2????????????????????????2
				Type[a][DTWO] = matchs(sboard[a][i], "000112") + matchs(sboard[a][i], "000113") + matchs(sboard[a][i], "0010112") + matchs(sboard[a][i], "001013") + matchs(sboard[a][i], "010012") + matchs(sboard[a][i], "010013") + matchs(sboard[a][i], "10001") + matchs(sboard[a][i], "2010102") + matchs(sboard[a][i], "3010102") + matchs(sboard[a][i], "3010103") + matchs(sboard[a][i], "2011002") + matchs(sboard[a][i], "2011003") + matchs(sboard[a][i], "3011002"); //???2???????????????3???2
				Type[a][SIFOUR] = matchs(sboard[a][i], "211112") + matchs(sboard[a][i], "211113") + matchs(sboard[a][i], "311113");																																																																																				  //???4
				Type[a][SITHREE] = matchs(sboard[a][i], "21112") + matchs(sboard[a][i], "21113") + matchs(sboard[a][i], "31113");																																																																																				  //???3
				Type[a][SITWO] = matchs(sboard[a][i], "2112") + matchs(sboard[a][i], "2113") + matchs(sboard[a][i], "3113");																																																																																					  //??????
			}
			else if (player == White)
			{
				Type[a][LONG] = matchs(sboard[a][i], "222222") || matchs(sboard[a][i], "2222222") || matchs(sboard[a][i], "22222222") || matchs(sboard[a][i], "222222222") || matchs(sboard[a][i], "2222222222") || matchs(sboard[a][i], "22222222222") || matchs(sboard[a][i], "222222222222") || matchs(sboard[a][i], "2222222222222") || matchs(sboard[a][i], "22222222222222") || matchs(sboard[a][i], "222222222222222");
				Type[a][FIVE] = matchs(sboard[a][i], "22222");																																																																																																					  //???5
				Type[a][FOUR] = matchs(sboard[a][i], "022220");																																																																																																					  //??????????????????????????????5
				Type[a][SFOUR] = matchs(sboard[a][i], "20222") + matchs(sboard[a][i], "22022") + matchs(sboard[a][i], "322220") + matchs(sboard[a][i], "122220");																																																																												  //???2?????????????????????
				Type[a][THREE] = matchs(sboard[a][i], "002220") / 2 + matchs(sboard[a][i], "020220");																																																																																											  //??????????????????????????????
				Type[a][DTHREE] = matchs(sboard[a][i], "122200") + matchs(sboard[a][i], "322200") + matchs(sboard[a][i], "122020") + matchs(sboard[a][i], "322020") + matchs(sboard[a][i], "120220") + matchs(sboard[a][i], "320220") + matchs(sboard[a][i], "22002") + matchs(sboard[a][i], "20202") + matchs(sboard[a][i], "1022201") + matchs(sboard[a][i], "3022201") + matchs(sboard[a][i], "3022203");																	  //??????????????????????????????
				Type[a][TWO] = matchs(sboard[a][i], "002200") + matchs(sboard[a][i], "020200") / 2 + matchs(sboard[a][i], "020020");																																																																																			  //???1????????????????????????1
				Type[a][DTWO] = matchs(sboard[a][i], "000221") + matchs(sboard[a][i], "000223") + matchs(sboard[a][i], "0020221") + matchs(sboard[a][i], "002023") + matchs(sboard[a][i], "020021") + matchs(sboard[a][i], "020023") + matchs(sboard[a][i], "20002") + matchs(sboard[a][i], "1020201") + matchs(sboard[a][i], "3020201") + matchs(sboard[a][i], "3020203") + matchs(sboard[a][i], "1022001") + matchs(sboard[a][i], "1022003") + matchs(sboard[a][i], "3022001"); //???1???????????????3???1
				Type[a][SIFOUR] = matchs(sboard[a][i], "122221") + matchs(sboard[a][i], "122223") + matchs(sboard[a][i], "322223");																																																																																				  //???2
				Type[a][SITHREE] = matchs(sboard[a][i], "12221") + matchs(sboard[a][i], "12223") + matchs(sboard[a][i], "32223");																																																																																				  //???3
				Type[a][SITWO] = matchs(sboard[a][i], "1221") + matchs(sboard[a][i], "1223") + matchs(sboard[a][i], "3223");																																																																																					  //??????
			}
		}
	}

	for (j = 0; j < 11; j++)
		type[j] = Type[0][j] + Type[1][j] + Type[2][j] + Type[3][j];
	//design ????????????????????????
	score = (type[FIVE] || type[LONG]) * 1000000 + type[FOUR] * 100000 + type[SFOUR] * 5000 + type[THREE] * 2000 + type[DTHREE] * 500 + type[TWO] * 50 + type[DTWO] * 30;
	for (boardscore = 0, i = 0; i < 15; i++)
	{
		for (j = 0; j < 15; j++)
		{
			boardscore += (board[i][j] == player) * MAPSCORE[i][j];
		}
	}
	return score + boardscore;
}

int evaluate(int board[15][15], int player)
{
	int allscore;
	allscore = evaluatea(board, player) - evaluatea(board, 3 - player);
	return allscore;
}
