#ifndef _HEAD_H
#define _HEAD_H
#include <stdlib.h>
#define AI_SEARCH_DEPTH 3

#define Black 1
#define White 2

#define LONG 0
#define FIVE 1
#define FOUR 2
#define SFOUR 3
#define THREE 4
#define DTHREE 5
#define TWO 6
#define DTWO 7
#define SIFOUR 8
#define SITHREE 9
#define SITWO 10
#define max(a, b) ((a) < (b)) ? (b) : (a)
#define min(a, b) ((a) < (b)) ? (a) : (b)

typedef struct Point
{
	int x;
	int y;
} Point;

typedef struct TreeNode
{
	struct TreeNode * right;
	struct TreeNode * left;
	int val;
}TreeNode;

typedef struct Spoint
{
	int x;
	int y;
	int score;
} Spoint;

void GenMove(int player, int board[15][15], Spoint moves[]);
//int Asearch(int board[15][15], int player, int depth, int alpha, int beta);
Spoint *search(int board[15][15], int player, int depth);
int HasNeighbor(int board[15][15], Point *p, int radius);
int cmp_Spoint(const void *_a, const void *_b);
void gentboard(int board[15][15], int t1board[17][17]);
int mygetline(Point *p, int a, char x[34]);
int match(char s[], char s1[], int e);
void chesstype(char s[], int e, int a, int player, int Type[4][11]);
void alltypes(int board[15][15], Point *p, int player, int type[11]);
int marking(int board[15][15], Point *p, int player);
int EvaluateChess(int board[15][15], Point *p, int player);
int matchs(char s[], char s1[]);
void gensboard(int t1board[17][17], char s1board[4][33][17]);
int evaluatea(int board[15][15], int player);
int evaluate(int board[15][15], int player);
int checkboard (int board[15][15]);
#endif
