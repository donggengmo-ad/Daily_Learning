#ifndef _GOBANG_H_
#define _GOBANG_H_

//棋盘常数
#define HEIGHT 600
#define WIDTH 600//棋盘长宽
#define GAP 30//行间距
//优势分常数
#define FIVE 100000        // 成五（直接拉满）
#define LIVE_FOUR_PER 2500 // 活四每颗：4颗×2500=10000
#define FOUR_PER 250       // 冲四每颗：4颗×250=1000
#define LIVE_THREE_PER 333 // 活三每颗：3颗×333≈1000
#define THREE_PER 33     // 冲三每颗：3颗×33≈100
#define LIVE_TWO_PER 50  // 活二每颗：2颗×50=100
#define TWO_PER 5        // 冲二每颗：2颗×5=10
#define ONE 1            // 单棋

int currentColor;//行动中颜色，1黑-1白
int playerColor;//玩家颜色，1黑-1白
int board[19][19];//棋盘上每个位置，0无子1黑子-1白子
int scoreBlack,scoreWhite;
int roundCount;//回合计数器（仅用于给AI判断全空棋盘）

void drawChessBoard();
void startGame();
int playerMove(int);
double dist(int,int,int,int);
int scoreCalc(int);
int checkLine(int,int,int,int,int);
int showMenu();
int doublePlayer();
int singlePlayer();
int AIMove();
void drawBattle();

#endif 