#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#define BOARD_SIZE 12
#define EMPTY 0
#define BLACK 1
#define WHITE 2
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define START "START"
#define PLACE "PLACE"
#define TURN "TURN"
#define END "END"

#define ENEMY_LIVE2 10
#define MY_LIVE2 10
#define ENEMY_DEAD3 30
#define MY_DEAD3 25
#define ENEMY_LIVE3 40
#define MY_LIVE3 50
#define ENEMY_DEAD4 60
#define MY_DEAD4 55
#define ENEMY_LIVE4 200
#define MY_LIVE4 10000
#define ENEMY_LIVE5 20000
#define MY_LIVE5 30000
#define max_depth 2

typedef struct {
	int first;
	int second;
} Pair;

Pair make_pair(int x, int y) {
	Pair p;
	p.first = x;
	p.second = y;
	return p;
}

struct Command
{
	int x;
	int y;
};
int step;
int in_2[BOARD_SIZE][BOARD_SIZE] = { 0 };
char board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int boardscore[BOARD_SIZE][BOARD_SIZE] = { 0 };
int myFlag;
int enemyFlag;
Pair max_position;
void judge_in_2() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] != EMPTY) continue;
			for (int y = -1; y <= 1; y++) {
				for (int x = -1; x <= 1; x++) {
					if (!(x == 0 && y == 0)) {
						if (board[i + y][j + x] != EMPTY/* || board[i + 2 * y][j + 2 * x] != EMPTY*/) {
							in_2[i][j] = 1;
							break;
						}
					}
				}
				if (in_2[i][j] != 0)
					break;
			}
		}
	}
}
void display()
{
	int i, j;
	printf("  ");
	for (i = 0; i < BOARD_SIZE; i++)
	{
		printf("%2d", i);
	}
	printf("\n");
	for (i = 0; i < BOARD_SIZE; i++)
	{
		printf("%2d", i);
		for (j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] == 0)
			{
				printf("┼ ");
			}
			else if (board[i][j] == 1)
			{
				printf("○");
			}
			else
			{
				printf("●");
			}
		}
		printf("\n");
	}
	printf("\n");
}
Pair find_max_position(int matrix[BOARD_SIZE][BOARD_SIZE]) {
	int max_val = INT_MIN;
	Pair max_pos;

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (matrix[i][j] > max_val) {
				max_val = matrix[i][j];
				max_pos = make_pair(i, j);
			}
		}
	}

	return max_pos;
}
void debug(const char* str)
{
	printf("DEBUG %s\n", str);
	fflush(stdout);
}
BOOL isInBound(int x, int y)
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}
void place(struct Command cmd)
{
	board[cmd.x][cmd.y] = enemyFlag;
}
BOOL isJump4(int me, int row, int col) {
	int enemy = 3 - me;
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (!(x == 0 && y == 0)) {
				if (isInBound(row + x * 5, col + y * 5)
					&& board[row + x * 5][col + y * 5] == enemy
					&& board[row + x * 4][col + y * 4] == me
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == EMPTY
					&& board[row - x][col - y] == EMPTY)
					return 1;
			}
		}
	}
	return 0;
}
BOOL isDEAD4(int me, int row, int col) {
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (!(x == 0 && y == 0)) {
				if (isInBound(row + x * 5, col + y * 5)
					&& board[row + x * 5][col + y * 5] == me
					&& board[row + x * 4][col + y * 4] == me
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == 3 - me
					&& board[row][col] == EMPTY)
					return 1;
			}
		}
	}
	return 0;
}
// 判断"三三"
BOOL isThreeThree(int me, int row, int col) {
	int count = 0; // 记录活三的数量
	if (board[row][col] != EMPTY)
		return 0;
	int a[10] = { 0 };
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (!(x == 0 && y == 0)) {
				if (((isInBound(row + x * 4, col + y * 4)
					&& board[row + x * 4][col + y * 4] == EMPTY)
					|| (isInBound(row - 2 * x, col - 2 * y)
						&& board[row - 2 * x][col - 2 * y] == EMPTY))
					&& board[row + x * 3][col + y * 3] == EMPTY
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == me
					&& board[row - x][col - y] == EMPTY) {
					a[0]++;
					count++;
				}

				if (isInBound(row + x * 3, col + y * 3)
					&& isInBound(row - x, col - y)
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == EMPTY
					&& board[row - x][col - y] == me) {
					a[1]++;
					count++;
				}
				if (isInBound(row + x * 4, col + y * 4)
					&& board[row + x * 4][col + y * 4] != 3 - me
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == EMPTY
					) {
					a[2]++;
					count++;
				}
				// 1 2重合
				if (isInBound(row + x * 4, col + y * 4)
					&& board[row + x * 4][col + y * 4] != 3 - me
					&& isInBound(row - x, col - y)
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == EMPTY
					&& board[row - x][col - y] == me) {
					count--;
				}
				if (isInBound(row + x * 3, col + y * 3)
					&& board[row - x * 2][col - y * 2] == EMPTY
					&& board[row + x * 3][col + y * 3] == EMPTY
					&& board[row + x * 2][col + y * 2] == EMPTY
					&& board[row + x][col + y] == me
					&& board[row - x][col - y] == me) {
					a[3]++;
					count++;
				}
				if (((isInBound(row + x * 3, col + y * 3)
					&& board[row + x * 3][col + y * 3] == EMPTY)
					|| (isInBound(row - 3 * x, col - 3 * y)
						&& board[row - 3 * x][col - 3 * y] == EMPTY))
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == EMPTY
					&& board[row - x][col - y] == me
					&& board[row - x * 2][col - y * 2] == EMPTY) {
					a[4]++;
					count++;
				}
				if (isInBound(row - 3 * x, col - 3 * y)
					&& board[row - 3 * x][col - 3 * y] == EMPTY
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == EMPTY
					&& board[row - x][col - y] == me
					&& board[row - x * 2][col - y * 2] == EMPTY) {
					count--;
				}
				if (((isInBound(row + x * 4, col + y * 4)
					&& board[row + x * 4][col + y * 4] == EMPTY)
					|| (isInBound(row - 3 * x, col - 3 * y)
						&& board[row - 3 * x][col - 3 * y] == EMPTY))
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == EMPTY
					&& board[row + x][col + y] == me
					&& board[row - x][col - y] == EMPTY
					&& board[row - x * 2][col - y * 2] == me
					)
					count--;

				if (((isInBound(row + x * 3, col + y * 3)
					&& board[row + x * 3][col + y * 3] == EMPTY)
					|| (isInBound(row - 2 * x, col - 2 * y)
						&& board[row - 2 * x][col - 2 * y] == EMPTY))
					&& board[row - x][col - y] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == me
					) {
					a[6]++;
					count++;
				}
				if (((isInBound(row + x * 4, col + y * 4)
					&& board[row + x * 4][col + y * 4] == EMPTY)
					|| (isInBound(row - x, col - y)
						&& board[row - x][col - y] == EMPTY))
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] != 3 - me
					&& board[row + x][col + y] == me
					) {
					a[7]++;
					count++;
				}

			}
		}
	}
	if (a[3] >= 2) {
		count = count - a[3] / 2;
	}
	/*if (count >= 2) {
		for (int i = 0; i < 9; i++)
			printf("%d", a[i]);
		printf("\n");
	}*/
	// 如果存在两个不同方向的活三，返回1，否则返回0
	return (count >= 2);
}
BOOL isLive4(int me, int row, int col) {
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (!(x == 0 && y == 0)) {
				if (isInBound(row + x * 4, col + y * 4)
					&& board[row + x * 4][col + y * 4] == me
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == me
					&& board[row][col] == EMPTY) {
					return 1;
				}
				else if (isInBound(row + x * 3, col + y * 3)
					&& isInBound(row - x, col - y)
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x * 1][col + y * 1] == me
					&& board[row - x][col - y] == me
					&& board[row][col] == EMPTY) {
					return 1;
				}
				else if (isInBound(row + x * 2, col + y * 2)
					&& isInBound(row - x * 2, col - y * 2)
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x * 1][col + y * 1] == me
					&& board[row - x * 2][col - y * 2] == me
					&& board[row - x][col - y] == me
					&& board[row][col] == EMPTY) {
					return 1;
				}
			}
		}
	}
	return 0;
}
int isLive3(int me, int row, int col) {
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (!(x == 0 && y == 0)) {
				if (isInBound(row - x * 3, col - y * 3)
					&& board[row - x * 3][col - y * 3] == EMPTY
					&& board[row + x * 2][col + y * 2] == EMPTY
					&& board[row - x * 2][col - y * 2] == EMPTY
					&& board[row][col] == me
					&& board[row + x][col + y] == me
					&& board[row - x][col - y] == me)
					return 1;
				else if (isInBound(row + x * 3, col + y * 3)
					&& isInBound(row - x * 2, col - y * 2)
					&& board[row + x * 1][col + y * 1] == me
					&& board[row + x * 3][col + y * 3] == EMPTY
					&& board[row + x * 2][col + y * 2] == me
					&& board[row - x * 2][col - y * 2] == EMPTY
					&& board[row][col] == EMPTY
					&& board[row - x][col - y] == me)
					return 2;
			}
		}
	}
	return 0;
}
BOOL isJump3(int me, int row, int col) {
	int enemy = 3 - me;
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (!(x == 0 && y == 0)) {
				if (isInBound(row + x * 5, col + y * 5)
					&& board[row + x * 4][col + y * 4] == EMPTY
					&& board[row + x * 3][col + y * 3] == me
					&& board[row + x * 2][col + y * 2] == me
					&& board[row + x][col + y] == EMPTY
					&& board[row - x][col - y] == EMPTY)
					return 1;
			}
		}
	}
	return 0;
}

void initAI(int me)
{
	enemyFlag = 3 - me;
}
int calculateall() {
	int myNum = 0, enemyNum = 0, emptyNum = 0;
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			boardscore[i][j] = 0;
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			//清零
			enemyNum = 0;
			myNum = 0;
			emptyNum = 0;
			//对每个点进行计算
			if (board[row][col] != EMPTY) continue;
			if (isJump3(enemyFlag, row, col))
				boardscore[row][col] -= ENEMY_LIVE3 - ENEMY_LIVE2 - 1;
			if (isJump4(enemyFlag, row, col))
				boardscore[row][col] -= ENEMY_DEAD4 - ENEMY_LIVE3 - 1;

			if (isJump3(myFlag, row, col))
				boardscore[row][col] += MY_LIVE3 - MY_LIVE2 - 1;
			if (isJump4(myFlag, row, col))
				boardscore[row][col] += MY_DEAD4 - MY_LIVE3 - 1;
			for (int y = -1; y <= 0; y++)
			{
				for (int x = -1; x <= 1; x++)
				{
					if (y == 0 && x == 0)continue;//只有四个方向
					if (y == 0 && x != 1)continue;
					enemyNum = 0;
					myNum = 0;
					emptyNum = 0;
					//假设敌人在该位置落棋，会构成什么棋型
					for (int i = 1; i <= 4; i++)
					{
						int curROW = row + i * y;//按方向计算的行数
						int curCOL = col + i * x;//列数

						if (isInBound(curROW, curCOL)
							&& board[curROW][curCOL] == enemyFlag)
						{
							enemyNum++;
						}
						else if (isInBound(curROW, curCOL)
							&& board[curROW][curCOL] == 0)
						{
							emptyNum++;
							break;
						}
						else break;
					}

					//反向判断
					for (int i = 1; i <= 4; i++)
					{
						int curROW = row - i * y;//按方向计算的行数
						int curCOL = col - i * x;//列数

						if (isInBound(curROW, curCOL)
							&& board[curROW][curCOL] == enemyFlag)
						{
							enemyNum++;
						}
						else if (isInBound(curROW, curCOL)
							&& board[curROW][curCOL] == 0)
						{
							emptyNum++;
							break;
						}
						else break;
					}

					if (enemyNum == 1)//连2
					{
						boardscore[row][col] -= 10;
					}
					else if (enemyNum == 2)
					{
						if (emptyNum == 1)
						{
							boardscore[row][col] -= 30;
						}
						else if (emptyNum == 2)
						{
							boardscore[row][col] -= 40;
						}
					}
					else if (enemyNum == 3)
					{
						if (emptyNum == 1)
						{
							boardscore[row][col] -= 60;
						}
						else if (emptyNum == 2)
						{
							boardscore[row][col] -= 2000;
						}
					}
					else if (enemyNum == 4)
					{
						boardscore[row][col] -= 20000;
					}
					//清零
					emptyNum = 0;
					//假设ai在该位置落棋，会构成什么棋型
					for (int i = 1; i <= 4; i++)
					{
						int curROW = row + i * y;//按方向计算的行数
						int curCOL = col + i * x;//列数

						if (isInBound(curROW, curCOL)
							&& board[curROW][curCOL] == myFlag)
						{
							myNum++;
						}
						else if (isInBound(curROW, curCOL)
							&& board[curROW][curCOL] == 0)
						{
							emptyNum++;
							break;
						}
						else break;
					}

					for (int i = 1; i <= 4; i++)
					{
						int curROW = row - i * y;//按方向计算的行数
						int curCOL = col - i * x;//列数

						if (isInBound(curROW, curCOL)
							&& board[curROW][curCOL] == myFlag)
						{
							myNum++;
						}
						else if (isInBound(curROW, curCOL)
							&& board[curROW][curCOL] == 0)
						{
							emptyNum++;
							break;
						}
						else break;
					}
					if (myNum == 0)
					{
						// boardscore[row][col] += 5;
					}
					else if (myNum == 1)
					{
						boardscore[row][col] += 10;
					}
					else if (myNum == 2)
					{
						if (emptyNum == 1)
						{
							boardscore[row][col] += 25;
						}
						else if (emptyNum == 2)
						{
							boardscore[row][col] += 50;
						}
					}
					else if (myNum == 3)
					{
						if (emptyNum == 1)
						{
							boardscore[row][col] += 55;
						}
						else if (emptyNum == 2)
						{
							boardscore[row][col] += 10000;
						}
					}
					else if (myNum >= 4)
					{
						boardscore[row][col] += 30000;
					}
				}
			}
		}
	}
	int score = 0;
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			score += boardscore[i][j];
	return score;
}
void add_in_2(int i, int j) {
	for (int y = -1; y <= 1; y++)
		for (int x = -1; x <= 1; x++) {
			if (isInBound(i + y, j + x))
				in_2[i + y][j + x]++;
			/*if (isInBound(i + 2 * y, j + 2 * x))
				in_2[i + 2*y][j + 2*x]++;*/
		}
}
void delete_in_2(int i, int j) {
	for (int y = -1; y <= 1; y++)
		for (int x = -1; x <= 1; x++) {
			if (isInBound(i + y, j + x))
				in_2[i + y][j + x]--;
			/*if (isInBound(i + 2 * y, j + 2 * x))
				in_2[i + 2 * y][j + 2 * x]--;*/
		}
}
int alphabeta(int depth, int alpha, int beta, int player) {
	if (depth == 0) {
		return calculateall();
	}
	if (player == myFlag) {
		for (int i = 0; i < BOARD_SIZE; i++)
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (board[i][j] == EMPTY && in_2[i][j] != 0) {
					board[i][j] = myFlag;
					add_in_2(i, j);
					int val = alphabeta(depth - 1, alpha, beta, 3 - player);
					if (val >= beta) {
						delete_in_2(i, j);
						board[i][j] = EMPTY;
						return beta;
					}
					if (val > alpha) {
						if (depth == max_depth)
						{
							//printf("己方价值%d 位置%d %d\n", val, i, j);
							max_position.first = i;
							max_position.second = j;
						}
						alpha = val;
					}
					delete_in_2(i, j);
					board[i][j] = EMPTY;
				}
			}
		return alpha;
	}
	else {
		for (int i = 0; i < BOARD_SIZE; i++)
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (board[i][j] == EMPTY && in_2[i][j] != 0) {
					board[i][j] = enemyFlag;
					add_in_2(i, j);
					int val = alphabeta(depth - 1, alpha, beta, 3 - player);
					if (val <= alpha) {
						delete_in_2(i, j);
						board[i][j] = EMPTY;
						return alpha;
					}
					//printf("对方价值%d 位置%d %d\n", val, i, j);
					if (val < beta) {
						beta = val;
					}
					delete_in_2(i, j);
					board[i][j] = EMPTY;
				}
			}
		return beta;
	}
}
void start(int flag)
{
	memset(board, 0, sizeof(board));
	int middlePlace = BOARD_SIZE / 2;
	board[middlePlace - 1][middlePlace - 1] = WHITE;
	board[middlePlace][middlePlace] = WHITE;
	board[middlePlace - 1][middlePlace] = BLACK;
	board[middlePlace][middlePlace - 1] = BLACK;
	initAI(flag);
}

void turn()
{
	int i, j, x, y;
	int TypeOfLive3 = 0;
	Pair enemyLive3Pos = { -1, -1 };  // 记录对方活三所在的位置
	Pair myLive3Pos = { -1, -1 };//记录自己活三所在位置
	Pair enemyLive4Pos = { -1, -1 };
	Pair myLive4Pos = { -1, -1 };
	Pair enemyThreeThree = { -1, -1 };
	Pair myThreeThree = { -1, -1 };


	//所有特判检测
	{
		//对方和自己活四检测
		{
			for (i = 0; i < BOARD_SIZE; i++) {
				for (j = 0; j < BOARD_SIZE; j++) {
					if (isLive4(myFlag, i, j)) {
						myLive4Pos = make_pair(i, j);
						break;
					}
				}
				if (myLive4Pos.first != -1 && myLive4Pos.second != -1) {
					break;
				}
			}
			if (myLive4Pos.first == -1 && myLive4Pos.second == -1) {
				for (i = 0; i < BOARD_SIZE; i++) {
					for (j = 0; j < BOARD_SIZE; j++) {
						if (isLive4(enemyFlag, i, j)) {
							enemyLive4Pos = make_pair(i, j);
							break;
						}
					}
					if (enemyLive4Pos.first != -1 && enemyLive4Pos.second != -1) {
						break;
					}
				}
			}
		}

		//对方和自己活三检测
		{
			for (i = 0; i < BOARD_SIZE; i++) {
				for (j = 0; j < BOARD_SIZE; j++) {
					if (TypeOfLive3 = isLive3(myFlag, i, j)) {
						myLive3Pos = make_pair(i, j);
						break;
					}
				}
				if (myLive3Pos.first != -1 && myLive3Pos.second != -1) {
					break;
				}
			}
			if (myLive3Pos.first == -1 && myLive3Pos.second == -1) {
				for (i = 0; i < BOARD_SIZE; i++) {
					for (j = 0; j < BOARD_SIZE; j++) {
						if (TypeOfLive3 = isLive3(enemyFlag, i, j)) {
							enemyLive3Pos = make_pair(i, j);
							break;
						}
					}
					if (enemyLive3Pos.first != -1 && enemyLive3Pos.second != -1) {
						break;
					}
				}
			}
		}

		//对方和自己三三检测
		{
			for (i = 0; i < BOARD_SIZE; i++) {
				for (j = 0; j < BOARD_SIZE; j++) {
					if (isThreeThree(myFlag, i, j)) {
						myThreeThree = make_pair(i, j);
						break;
					}
				}
				if (myThreeThree.first != -1 && myThreeThree.second != -1) {
					break;
				}
			}
			if (myThreeThree.first == -1 && myThreeThree.second == -1) {
				for (i = 0; i < BOARD_SIZE; i++) {
					for (j = 0; j < BOARD_SIZE; j++) {
						if (isThreeThree(enemyFlag, i, j)) {
							enemyThreeThree = make_pair(i, j);
							break;
						}
					}
					if (enemyThreeThree.first != -1 && enemyThreeThree.second != -1) {
						break;
					}
				}
			}
		}
	}

	calculateall();
	//所有特判处理
	{
		if (!(myLive4Pos.first == -1 && myLive4Pos.second == -1)) {
			printf("%d %d", myLive4Pos.first, myLive4Pos.second);
			printf("\n");
			fflush(stdout);
			board[myLive4Pos.first][myLive4Pos.second] = myFlag;
			return;
		}
		//printf("1\n");

		if (!(enemyLive4Pos.first == -1 && enemyLive4Pos.second == -1)) {
			printf("%d %d", enemyLive4Pos.first, enemyLive4Pos.second);
			printf("\n");
			fflush(stdout);
			board[enemyLive4Pos.first][enemyLive4Pos.second] = myFlag;
			return;
		}
		//printf("2\n");

		if (!(myLive3Pos.first == -1 && myLive3Pos.second == -1)) {
			if (TypeOfLive3 == 2) {
				printf("%d %d", myLive3Pos.first, myLive3Pos.second);
				printf("\n");
				fflush(stdout);
				board[myLive3Pos.first][myLive3Pos.second] = myFlag;
				return;
			}
			else {
				int x1 = 0, y1 = 0;
				for (y = -1; y <= 1; y++) {
					for (x = -1; x <= 1; x++) {
						if (!(x == 0 && y == 0)) {
							if (isInBound(myLive3Pos.first + x * 3, myLive3Pos.second + y * 3)
								&& board[myLive3Pos.first + x * 3][myLive3Pos.second + y * 3] == EMPTY
								&& board[myLive3Pos.first + x * 2][myLive3Pos.second + y * 2] == EMPTY
								&& board[myLive3Pos.first - x * 2][myLive3Pos.second - y * 2] == EMPTY
								&& board[myLive3Pos.first][myLive3Pos.second] == myFlag
								&& board[myLive3Pos.first + x][myLive3Pos.second + y] == myFlag
								&& board[myLive3Pos.first - x][myLive3Pos.second - y] == myFlag) {
								x1 = x; y1 = y;
								break;
							}
						}
					}
					if (!(x1 == 0 && y1 == 0))
						break;
				}
				if (!isInBound(myLive3Pos.first - x1 * 3, myLive3Pos.second - y1 * 3)) {
					int Max = INT_MIN;
					max_position.first = max_position.second = -1;
					alphabeta(max_depth, INT_MIN, INT_MAX, myFlag);
					printf("%d %d", max_position.first, max_position.second);
					printf("\n");
					fflush(stdout);
					board[max_position.first][max_position.second] = myFlag;
				}
				else if (boardscore[myLive3Pos.first + x1 * 2][myLive3Pos.second + y1 * 2] > boardscore[myLive3Pos.first - x1 * 2][myLive3Pos.second - y1 * 2]) {
					printf("%d %d", myLive3Pos.first + x1 * 2, myLive3Pos.second + y1 * 2);
					printf("\n");
					fflush(stdout);
					board[myLive3Pos.first + x1 * 2][myLive3Pos.second + y1 * 2] = myFlag;
				}
				else {
					printf("%d %d", myLive3Pos.first - x1 * 2, myLive3Pos.second - y1 * 2);
					printf("\n");
					fflush(stdout);
					board[myLive3Pos.first - x1 * 2][myLive3Pos.second - y1 * 2] = myFlag;
				}
			}

			return;
		}
		//printf("3\n");

		if (!(enemyLive3Pos.first == -1 && enemyLive3Pos.second == -1)) {
			if (TypeOfLive3 == 2) {
				printf("%d %d", enemyLive3Pos.first, enemyLive3Pos.second);
				printf("\n");
				fflush(stdout);
				board[enemyLive3Pos.first][enemyLive3Pos.second] = myFlag;
				return;
			}
			else {
				int x1 = 0, y1 = 0;
				//printf("活三位置%d %d\n", enemyLive3Pos.first, enemyLive3Pos.second);
				for (y = -1; y <= 1; y++) {
					for (x = -1; x <= 1; x++) {
						if (!(x == 0 && y == 0)) {
							if (isInBound(enemyLive3Pos.first + x * 3, enemyLive3Pos.second + y * 3)
								&& board[enemyLive3Pos.first + x * 3][enemyLive3Pos.second + y * 3] == EMPTY
								&& board[enemyLive3Pos.first + x * 2][enemyLive3Pos.second + y * 2] == EMPTY
								&& board[enemyLive3Pos.first - x * 2][enemyLive3Pos.second - y * 2] == EMPTY
								&& board[enemyLive3Pos.first][enemyLive3Pos.second] == enemyFlag
								&& board[enemyLive3Pos.first + x][enemyLive3Pos.second + y] == enemyFlag
								&& board[enemyLive3Pos.first - x][enemyLive3Pos.second - y] == enemyFlag) {
								//printf("xy分别为%d %d\n", x, y);
								x1 = x; y1 = y;
								break;
							}
						}
					}
					if (!(x1 == 0 && y1 == 0))
						break;
				}

				if (!isInBound(enemyLive3Pos.first - x1 * 3, enemyLive3Pos.second - y1 * 3)) {
					//printf("活三位置%d %d\n", enemyLive3Pos.first, enemyLive3Pos.second);
					//printf("xy分别为%d %d\n", x1, y1);
					printf("%d %d", enemyLive3Pos.first + x1 * 3, enemyLive3Pos.second + y1 * 3);
					printf("\n");
					fflush(stdout);
					board[enemyLive3Pos.first + x1 * 3][enemyLive3Pos.second + y1 * 3] = myFlag;
				}
				else if (board[enemyLive3Pos.first + x1 * 3][enemyLive3Pos.second + y1 * 3] == myFlag) {
					printf("%d %d", enemyLive3Pos.first - x1 * 2, enemyLive3Pos.second - y1 * 2);
					printf("\n");
					fflush(stdout);
					board[enemyLive3Pos.first - x1 * 2][enemyLive3Pos.second - y1 * 2] = myFlag;
				}
				else if (board[enemyLive3Pos.first - x1 * 3][enemyLive3Pos.second - y1 * 3] == myFlag) {
					printf("%d %d", enemyLive3Pos.first + x1 * 2, enemyLive3Pos.second + y1 * 2);
					printf("\n");
					fflush(stdout);
					board[enemyLive3Pos.first + x1 * 2][enemyLive3Pos.second + y1 * 2] = myFlag;
				}
				else if (boardscore[enemyLive3Pos.first + x1 * 2][enemyLive3Pos.second + y1 * 2] < boardscore[enemyLive3Pos.first - x1 * 2][enemyLive3Pos.second - y1 * 2]) {
					printf("%d %d", enemyLive3Pos.first + x1 * 2, enemyLive3Pos.second + y1 * 2);
					printf("\n");
					fflush(stdout);
					board[enemyLive3Pos.first + x1 * 2][enemyLive3Pos.second + y1 * 2] = myFlag;
				}
				else {
					printf("%d %d", enemyLive3Pos.first - x1 * 2, enemyLive3Pos.second - y1 * 2);
					printf("\n");
					fflush(stdout);
					board[enemyLive3Pos.first - x1 * 2][enemyLive3Pos.second - y1 * 2] = myFlag;
				}
			}

			return;
		}
		//printf("4\n");

		if (!(myThreeThree.first == -1 && myThreeThree.second == -1)) {
			printf("%d %d", myThreeThree.first, myThreeThree.second);
			printf("\n");
			fflush(stdout);
			board[myThreeThree.first][myThreeThree.second] = myFlag;
			return;
		}
		//printf("5\n");

		if (!(enemyThreeThree.first == -1 && enemyThreeThree.second == -1)) {
			printf("%d %d", enemyThreeThree.first, enemyThreeThree.second);
			printf("\n");
			fflush(stdout);
			board[enemyThreeThree.first][enemyThreeThree.second] = myFlag;
			return;
		}
		//printf("6\n");

		if (myFlag == WHITE && step == 8 && board[4][4] == BLACK && board[5][4] == BLACK && board[4][7] == WHITE) {
			printf("%d %d", 6, 4);
			printf("\n");
			fflush(stdout);
			board[6][4] = myFlag;
			return;
		}
		if (myFlag == WHITE && step == 8 && board[4][4] == BLACK && board[4][3] == BLACK && board[4][7] == WHITE) {
			printf("%d %d", 6, 4);
			printf("\n");
			fflush(stdout);
			board[6][4] = myFlag;
			return;
		}
		if (myFlag == WHITE && step == 8 && board[4][4] == BLACK && board[4][5] == BLACK && board[7][4] == WHITE) {
			printf("%d %d", 4, 6);
			printf("\n");
			fflush(stdout);
			board[4][6] = myFlag;
			return;
		}
		if (myFlag == WHITE && step == 8 && board[4][4] == BLACK && board[3][4] == BLACK && board[7][4] == WHITE) {
			printf("%d %d", 4, 6);
			printf("\n");
			fflush(stdout);
			board[4][6] = myFlag;
			return;
		}
		if (myFlag == WHITE && step == 8 && board[7][7] == BLACK && board[7][6] == BLACK && board[4][7] == WHITE) {
			printf("%d %d", 7, 5);
			printf("\n");
			fflush(stdout);
			board[7][5] = myFlag;
			return;
		}
		if (myFlag == WHITE && step == 8 && board[7][7] == BLACK && board[8][7] == BLACK && board[4][7] == WHITE) {
			printf("%d %d", 7, 5);
			printf("\n");
			fflush(stdout);
			board[7][5] = myFlag;
			return;
		}
		if (myFlag == WHITE && step == 8 && board[7][7] == BLACK && board[7][8] == BLACK && board[7][4] == WHITE) {
			printf("%d %d", 5, 7);
			printf("\n");
			fflush(stdout);
			board[5][7] = myFlag;
			return;
		}
	}


	//ab剪枝
	int Max = INT_MIN;
	max_position.first = max_position.second = -1;
	alphabeta(max_depth, INT_MIN, INT_MAX, myFlag);
	if (max_position.first == -1 || max_position.second == -1) {
		for (int i = 0; i < BOARD_SIZE; i++)
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (board[i][j] == EMPTY) {
					max_position.first = i;
					max_position.second = j;
					break;
				}
			}
	}
	printf("%d %d", max_position.first, max_position.second);
	printf("\n");
	fflush(stdout);
	board[max_position.first][max_position.second] = myFlag;
	return;
}
void end(int x)
{
	exit(0);
}

void loop()
{
	char tag[10] = { 0 };
	struct Command command =
	{
		.x = 0,
		.y = 0,
	};
	int status;
	while (TRUE)
	{
		memset(tag, 0, sizeof(tag));
		scanf("%s", tag);
		if (strcmp(tag, START) == 0)
		{
			step = 4;
			scanf("%d", &myFlag);
			start(myFlag);
			printf("OK\n");
			fflush(stdout);
		}
		else if (strcmp(tag, PLACE) == 0)
		{
			step++;
			scanf("%d %d", &command.x, &command.y);
			place(command);
		}
		else if (strcmp(tag, TURN) == 0)
		{
			step++;
			judge_in_2();
			turn();
		}
		else if (strcmp(tag, END) == 0)
		{
			scanf("%d", &status);
			end(status);
		}
		display();
	}
}
int main(int argc, char* argv[])
{
	loop();
	return 0;
}