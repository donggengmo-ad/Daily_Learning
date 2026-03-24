#include <graphics.h>
#include "Gobang.h"
#include <math.h>
#include <conio.h>

void drawChessBoard(){
	//打印棋盘背景
	setbkcolor(RGB(80,40,20)); 
	cleardevice();
	//打印天元星位
	setlinecolor(BLACK);
	setfillcolor(BLACK);
	//天元
	fillcircle(GAP * 10, GAP * 10, 4);
	//四隅
	fillcircle(GAP * 4, GAP * 4, 4);
	fillcircle(GAP * 16, GAP * 4, 4);
	fillcircle(GAP * 4, GAP * 16, 4);
	fillcircle(GAP * 16, GAP * 16, 4);
	//边角
	fillcircle(GAP * 10, GAP * 4, 4);
	fillcircle(GAP * 10, GAP * 16, 4);
	fillcircle(GAP * 4, GAP * 10, 4);
	fillcircle(GAP * 16, GAP * 10, 4);
	//打印棋盘线
	setlinestyle(PS_SOLID|PS_ENDCAP_FLAT,2);
	for(int i = 1;i <= 19;i++){
		line(GAP,i * GAP,WIDTH - GAP,i * GAP);
		line(i * GAP,GAP,i * GAP,HEIGHT - GAP);
	}
}
void drawBattle(){//绘制战况
	//按比例计算战况条长度
	int blackLong;
	/*
	blackLong + whiteLong = WIDTH
	blackLong / whiteLong = scoreBlack / scoreWhite
	==>
	blackLong = WIDTH * scoreBlack / (scoreBlack + scoreWhite)
	*/
	blackLong = WIDTH * scoreBlack / (scoreBlack + scoreWhite);
	//绘制黑棋战况条
	setlinecolor(BLACK);
	setfillcolor(BLACK);
	fillrectangle(0,HEIGHT,blackLong,HEIGHT + 10);
	//绘制白棋战况条
	setlinecolor(WHITE);
	setfillcolor(WHITE);
	fillrectangle(blackLong,HEIGHT,WIDTH,HEIGHT + 10);
}
void startGame(){
	//30像素一行，18行棋盘，周围边框
	initgraph(WIDTH,HEIGHT + 10);//多出10像素显示战况
	drawChessBoard();
	int i,j;
	//初始化棋盘（均未落子）
	for(i = 0;i < 19;i++){
		for(j = 0;j < 19;j++){
			board[i][j] = 0;
		}
	}
	//双方优势分归零
	scoreBlack = 0;
	scoreWhite = 0;
	//行动人设为黑棋
	currentColor = 1;
	roundCount = 0;
}
double dist(int x1,int y1,int x2,int y2){//两点距离
	double d;
	d = pow((x1 - x2),2) + pow((y1 -y2),2);
	d = sqrt(d);
	return d;
}
int playerMove(){//玩家落子，返回1表示成功落子并应切换颜色，0表示未落子
	ExMessage msg;
	msg = getmessage(EX_MOUSE);
	int i,j;
	static int tempx = -1,tempy = -1;//临时坐标
	for(j = 1;j <= 19;j++){
		for(i = 1;i <= 19;i++){
			//寻找附近空落点
			int x = GAP * i;
			int y = GAP * j;
			if(dist(x,y,msg.x,msg.y) < GAP / 2 && !board[i-1][j-1]){
				//按下左键落子
				if(msg.lbutton){
					//根据玩家颜色给棋子上色
					if(currentColor == 1){
						setlinecolor(BLACK);
						setfillcolor(BLACK);
					}
					else{
						setlinecolor(WHITE);
						setfillcolor(WHITE);
					}
					//落子
					fillcircle(x,y,10);
					board[i-1][j-1] = currentColor;
					return 1;
				}
				//没按键时预览
				else{
					if(x != tempx || y != tempy){//落点预览变化
						if(tempx == -1 && tempy == -1){//初次变化
							tempx = x;
							tempy = y;
						}
						else{
							if(!board[tempx/GAP - 1][tempy/GAP - 1]){//没有落子才还原
								//还原棋盘背景
								setlinecolor(RGB(80,40,20));
								setfillcolor(RGB(80,40,20));
								fillcircle(tempx,tempy,10);
								//还原棋盘格
								setlinecolor(BLACK);
								if(tempx == GAP && tempy == GAP){//左上角
									line(tempx,tempy,tempx + 11,tempy);//横格
									line(tempx,tempy,tempx,tempy + 11);//竖格
								}
								else if(tempx == GAP * 19 && tempy == GAP){//右上角
									line(tempx - 11,tempy,tempx,tempy);//横格
									line(tempx,tempy,tempx,tempy + 11);//竖格
								}
								else if(tempx == GAP && tempy == GAP * 19){//左下角
									line(tempx,tempy - 11,tempx,tempy);//竖格
									line(tempx,tempy,tempx + 11,tempy);//横格
								}
								else if(tempx == GAP * 19 && tempy == GAP * 19){//右下角
									line(tempx,tempy - 11,tempx,tempy);//竖格
									line(tempx - 11,tempy,tempx,tempy);//横格
								}
								else if(tempx == GAP || tempx == GAP * 19 || tempy == GAP || tempy == GAP * 19){//左右边界
									if(tempx == GAP){//左边界
										line(tempx,tempy - 11,tempx,tempy + 11);//竖格
										line(tempx,tempy,tempx + 11,tempy);//横格
									}
									else if(tempx == GAP * 19){//右边界
										line(tempx,tempy - 11,tempx,tempy + 11);//竖格
										line(tempx - 11,tempy,tempx,tempy);//横格
									}
									else if(tempy == GAP){//上边界
										line(tempx - 11,tempy,tempx + 11,tempy);//横格
										line(tempx,tempy,tempx,tempy + 11);//竖格
									}
									else if(tempy == GAP * 19){//下边界
										line(tempx - 11,tempy,tempx + 11,tempy);//横格
										line(tempx,tempy - 11,tempx,tempy);//竖格
									}
								}
								else{//棋盘中
									line(tempx - 11,tempy,tempx + 11,tempy);//横格
									line(tempx,tempy - 11,tempx,tempy + 11);//竖格
								}
								//还原天元星位
								setlinecolor(BLACK);
								setfillcolor(BLACK);
								if(tempx == GAP * 10 && tempy == GAP * 10){//天元
									fillcircle(tempx, tempy, 4);
								}
								else if(tempx == GAP * 4 && tempy == GAP * 4){//四隅1
									fillcircle(GAP * 4, GAP * 4, 4);
								}
								else if(tempx == GAP * 16 && tempy == GAP * 4){//四隅2
									fillcircle(GAP * 16, GAP * 4, 4);
								}
								else if(tempx == GAP * 4 && tempy == GAP * 16){//四隅3
									fillcircle(GAP * 4, GAP * 16, 4);
								}
								else if(tempx == GAP * 16 && tempy == GAP * 16){//四隅4
									fillcircle(GAP * 16, GAP * 16, 4);
								}
								else if(tempx == GAP * 10 && tempy == GAP * 4){//边角1
									fillcircle(GAP * 10, GAP * 4, 4);
								}
								else if(tempx == GAP * 10 && tempy == GAP * 16){//边角2
									fillcircle(GAP * 10, GAP * 16, 4);
								}
								else if(tempx == GAP * 4 && tempy == GAP * 10){//边角3
									fillcircle(GAP * 4, GAP * 10, 4);
								}
								else if(tempx == GAP * 16 && tempy == GAP * 10){//边角4
									fillcircle(GAP * 16, GAP * 10, 4);
								}
								//打印阴影
								if(currentColor == -1){//白棋预览偏亮
									setlinecolor(RGB(100,60,40));
									setfillcolor(RGB(100,60,40));
								}
								else{//黑棋预览偏暗
									setlinecolor(RGB(60,20,0));
									setfillcolor(RGB(60,20,0));
								}
								fillcircle(x,y,10);
							}
							//记录临时位置
							tempx = x;
							tempy = y;
						}
					}
				}
			}
		}
	}
	//未落子
	return 0;
}
int AIMove(){//AI落子，返回1表示成功落子并应切换颜色
	int i,j;
	//遍历棋盘求分数
	int maxScore = -1000000;
	int bestX = -1,bestY = -1;
	for(j = 0;j < 19;j++){
		for(i = 0;i < 19;i++){
			//算法优化：周围无子跳过（但不要在全空时触发）
			if(roundCount == 0){//全空时落天元
				bestX = 9;
				bestY = 9;
				roundCount++;
				break;
			}
			int emptyNearby = 0;
			for(int dj = -1;dj <= 1;dj++){
				for(int di = -1;di <= 1;di++){
					int tempx = i + di;
					int tempy = j + dj;
					//边界检查
					if(tempx < 0 || tempx >= 19 || tempy < 0 || tempy >= 19){
						continue;
					}
					if(board[tempx][tempy] != 0){
						emptyNearby = 1;
						break;
					}
				}
				if(emptyNearby){
					break;
				}
			}
			if(!emptyNearby){
				continue;
			}
			//非空位跳过
			if(board[i][j] != 0){
				continue;
			}
			//模拟落子
			board[i][j] = currentColor;
			//计算分数差
			int score;
			if(playerColor == 1){//玩家黑棋
				score = scoreCalc(-1) - 10 * scoreCalc(1);//加大防守权重
			}
			else{//玩家白棋
				score = scoreCalc(1) - 10 * scoreCalc(-1);//加大防守权重
			} 
			//记录最佳落子点
			if(score > maxScore){
				maxScore = score;
				bestX = i;
				bestY = j;
			}
			//还原棋盘
			board[i][j] = 0;
		}
	}
	//落子
	if(currentColor == 1){
		setlinecolor(BLACK);
		setfillcolor(BLACK);
	}
	else{
		setlinecolor(WHITE);
		setfillcolor(WHITE);
	}
	fillcircle(GAP * (bestX + 1), GAP * (bestY + 1), 10);
	board[bestX][bestY] = currentColor;
	return 1;
}
int checkLine(int x,int y,int dx,int dy,int color){//检查某一方向上的分数
	//记录我方棋子数、阻挡数
	int piece = 1;
	int block = 0;
	//朝正方向扫描
	for(int i = 1;;i++){
		//移动扫描点
		int tempx = x + i * dx;
		int tempy = y + i * dy;
		//边界检查
		if(tempx < 0 || tempx >= 19 || tempy < 0 || tempy >= 19){
			block++;
			break;
		}
		//检查棋子颜色
		if(board[tempx][tempy] == color){
			piece++;//我方棋子
		}
		else if(board[tempx][tempy] == 0){
			break;//空位
		}
		else{
			block++;
			break;//敌方棋子
		}
	}
	//朝反方向扫描
	dx *= -1;
	dy *= -1;
	for(int i = 1;;i++){
		//移动扫描点
		int tempx = x + i * dx;
		int tempy = y + i * dy;
		//边界检查
		if(tempx < 0 || tempx >= 19 || tempy < 0 || tempy >= 19){
			block++;
			break;
		}
		//检查棋子颜色
		if(board[tempx][tempy] == color){
			piece++;//我方棋子
		}
		else if(board[tempx][tempy] == 0){
			break;//空位
		}
		else{
			block++;
			break;//敌方棋子
		}
	}
	//合算分数
	if(piece >= 5){
		return FIVE;//赢棋
	}
	else if(block >= 2){
		return 0;//死棋
	}
	else if(block == 1){
		if(piece == 4){
			return FOUR_PER;//冲四
		}
		else if(piece == 3){
			return THREE_PER;//冲三
		}
		else if(piece == 2){
			return TWO_PER;//冲二
		}
		else{
			return ONE;//单棋
		}
	}
	else{
		if(piece == 4){
			return LIVE_FOUR_PER;//活四
		}
		else if(piece == 3){
			return LIVE_THREE_PER;//活三
		}
		else if(piece == 2){
			return LIVE_TWO_PER;//活二
		}
		else{
			return ONE;//单棋
		}
	}
}
int scoreCalc(int color){//计算某种颜色的优势分
	int total = 0;
	int i,j;
	for(j = 0;j < 19;j++){
		for(i = 0;i < 19;i++){
			if(board[i][j] == color){
				int horizontal = checkLine(i,j,1,0,color);//水平线
				int vertical = checkLine(i,j,0,1,color);//垂直线
				int leanLeft = checkLine(i,j,-1,1,color);//左倾斜
				int leanRight = checkLine(i,j,1,1,color);//右倾斜
				total += (horizontal + vertical + leanLeft + leanRight);
			}
		}
	}
	return total;
}
int showMenu(){
	//显示菜单界面
	initgraph(400,300);
	setbkcolor(BLACK);
	cleardevice();
	settextcolor(WHITE);
	TCHAR title[] = _T("五子棋");
	outtextxy(170,50,title);
	TCHAR m1[] = _T("1. 单人模式");
	outtextxy(170,150,m1);
	TCHAR m2[] = _T("2. 双人模式");
	outtextxy(170,200,m2);
	//获取模式
	int mode = 0;
	while(1){
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){
			if(msg.x >= 170 && msg.x <= 270){
				if(msg.y >= 150 && msg.y <= 180){
					mode = 1;//单人
					break;
				}
				else if(msg.y >= 200 && msg.y <= 230){
					mode = 2;//双人
					break;
				}
			}
		}
	}
	//单人模式继续选择玩家颜色
	if(mode == 1){
		cleardevice();
		TCHAR o1[] = _T("1.玩家先手");
		outtextxy(170,100,o1);
		TCHAR o2[] = _T("2.电脑先手");
		outtextxy(170,170,o2);
		//获取先手顺序
		while(1){
			ExMessage msg = getmessage(EX_MOUSE);
			if(msg.lbutton){
				if(msg.x >= 170 && msg.x <= 270){
					if(msg.y >= 100 && msg.y <= 130){
						playerColor = 1;//玩家黑
						break;
					}
					else if(msg.y >= 170 && msg.y <= 200){
						playerColor = -1;//玩家白
						break;
					}
				}
			}
		}
	}
	closegraph();
	return mode;
}
int doublePlayer(){//双人模式，返回1游戏结束，0游戏继续,-1退回上级
	if(playerMove()){//玩家成功落子
		ExMessage msg;
		while(peekmessage(&msg, EX_KEY)){
			if(msg.message==WM_KEYDOWN && msg.vkcode==VK_ESCAPE){
				return -1;
			}
		}
		//切换行动玩家
		currentColor *= -1;
		//计算双方优势分
		scoreBlack = scoreCalc(1);
		scoreWhite = scoreCalc(-1);
		drawBattle();//绘制战况
		//判断胜负
		if(scoreBlack >= FIVE){
			TCHAR s[] = _T("黑胜！");
			outtextxy(WIDTH / 2 - 10, HEIGHT / 2, s);
			return 1;
		}
		else if(scoreWhite >= FIVE){
			TCHAR s[] = _T("白胜！");
			outtextxy(WIDTH / 2 - 10, HEIGHT / 2, s);
			return 1;
		}
	}
	return 0;
}
int singlePlayer(){//单人模式，返回1游戏结束，0游戏继续
	ExMessage msg;
	while(peekmessage(&msg, EX_KEY)){
		if(msg.message==WM_KEYDOWN && msg.vkcode==VK_ESCAPE){
			return -1;
		}
	}
	//判断当前行动颜色
	if(playerColor == currentColor){
		if(playerMove()){//玩家成功落子
			//切换行动玩家
			currentColor *= -1;
			//计算双方优势分
			scoreBlack = scoreCalc(1);
			scoreWhite = scoreCalc(-1);
			drawBattle();//绘制战况
		}
	}
	else{
		if(AIMove()){//AI成功落子
			//切换行动玩家
			currentColor *= -1;
			//计算双方优势分
			scoreBlack = scoreCalc(1);
			scoreWhite = scoreCalc(-1);
			drawBattle();//绘制战况
		}
	}
	//判断胜负
	if(scoreBlack >= FIVE){
		TCHAR s[] = _T("黑胜！");
		outtextxy(WIDTH / 2 - 10, HEIGHT / 2, s);
		return 1;
	}
	else if(scoreWhite >= FIVE){
		TCHAR s[] = _T("白胜！");
		outtextxy(WIDTH / 2 - 10, HEIGHT / 2, s);
		return 1;
	}
	return 0;
}

int main(){
	int mode = showMenu();
	startGame();
	int result = 0;
	if (mode == 1){//单人模式
		while(!result){
			result = singlePlayer();
		}
	}
	else if(mode == 2){//双人模式
		while(!result){
			result = doublePlayer();
		}
	}
	//判断结束还是返回
	if(result == -1){
		main();//返回上级菜单
	}
	else if(result == 1){
		//游戏结束，等待按键退出
		getch();
		closegraph();
		return 0;
	}
}

