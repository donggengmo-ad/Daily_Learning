//游戏界面相关代码文件

#include "auction.h"
#include <stdio.h>
#include <wchar.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <math.h>

IMAGE background;//背景图片
IMAGE table;//圆桌图片
IMAGE closet;//柜子图片
IMAGE showcase1;//展示柜图片1
IMAGE showcase2;//展示柜图片2

//工具函数
void transparentimage(IMAGE* picture, int  picture_x, int picture_y){ //png透明图片
	// 变量初始化
	DWORD *dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD *draw = GetImageBuffer();
	DWORD *src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标
 
	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}
void PutImageZoom(int x, int y, int w, int h, IMAGE* srcImg){
	// 创建目标尺寸的兼容DC用于缩放操作
	IMAGE destImg(w,h);
	// 设置缩放模式为高质量
	SetStretchBltMode(GetImageHDC(&destImg), HALFTONE);
	// 使用StretchBlt进行高质量缩放
	StretchBlt(GetImageHDC(&destImg), 0, 0, w, h, GetImageHDC(srcImg), 0, 0, srcImg->getwidth(), srcImg->getheight(), SRCCOPY);
	putimage(x, y, &destImg);
}
TCHAR* CharToTCHAR(const char* pChar){//char*转换为TCHAR*，用完需delete[]
    if(pChar == NULL) return NULL;
#ifdef UNICODE
    // 获取需要的长度（包含结尾 '\0'）
    int nLen = MultiByteToWideChar(CP_ACP, 0, pChar, -1, NULL, 0);
    if(nLen <= 0) return NULL;
    wchar_t *pTchar = new wchar_t[nLen];
    if(!pTchar) return NULL;
    MultiByteToWideChar(CP_ACP, 0, pChar, -1, pTchar , nLen);
    return (TCHAR*)pTchar;
#else
    size_t nLen = strlen(pChar) + 1;
    char *pTchar = new char[nLen];
    if(!pTchar) return NULL;
    strcpy_s(pTchar, nLen, pChar);
    return (TCHAR*)pTchar;
#endif
}
void printPlayer(int x,int y,int player_no,int mode){//mode数组 0普通,1高亮,2带信息,12带信息且高亮,-1红框，-12红框且带信息
	int size = AVATAR_SIZE / 4;
	//显示玩家名称
	settextstyle(size,size / 2,_T("Calibri"));
	settextcolor(BLACK);
	TCHAR *player_name = CharToTCHAR(players[player_no].name);
	outtextxy(x,y + AVATAR_SIZE + size / 2, player_name);
	delete[] player_name;
	//显示玩家头像
	transparentimage(players[player_no].avatar,x, y);
	if(mode == 1 || mode == 12){
		//显示高亮框
		setlinecolor(YELLOW);
		setlinestyle(PS_DASH,2);
		rectangle(x - 2, y - 2, x + AVATAR_SIZE + 2, y + AVATAR_SIZE + 2);
	}
	if(mode == -1 || mode == -12){
		//显示红框
		setlinecolor(RED);
		setlinestyle(PS_DASH,2);
		rectangle(x - 2, y - 2, x + AVATAR_SIZE + 2, y + AVATAR_SIZE + 2);
	}
	if(mode == 2 || mode == 12){
		//显示玩家资产
		settextcolor(GREEN);
		TCHAR block[100];
		_itot(players[player_no].block, block, 10);
		outtextxy(x, y + AVATAR_SIZE + size + size / 2, block);
		outtextxy(x + size * 2, y + AVATAR_SIZE + size + size / 2, _T("$"));
		//显示玩家分数
		settextcolor(BLUE);
		TCHAR score[100];
		_itot(players[player_no].score, score, 10);
		outtextxy(x, y + AVATAR_SIZE + size * 2 + size / 2, score);
		outtextxy(x + size * 2, y + AVATAR_SIZE + size * 2 + size / 2, _T("Score"));
	}
}
void printBackground(int up = 0,int down = 0,int left = 0,char *title = NULL){//up:-1拍卖师i玩家编号 down:1卡牌2出价 left:1圆桌2柜子
	//清屏
	cleardevice();
	//打印背景
	putimage(0, 0, &background);
	//米黄色信息栏
	setlinecolor(RGB(255, 239, 204));
	setfillcolor(RGB(255, 239, 204));
	fillrectangle(WIDTH * 6 / 8, 0, WIDTH, HEIGHT);
	//打印标题
	if(title != NULL){
		settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
		settextcolor(BLACK);
		TCHAR *t = CharToTCHAR(title);
		outtextxy(WIDTH * 3 / 4 + 10, 20, t);
		delete[] t;
	}
	//打印左上
	if(up == -1){
		//打印拍卖师标题
		settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
		settextcolor(BLACK);
		outtextxy(WIDTH * 6 / 8 + 20, 60, _T("Auctioneer:"));
		//打印拍卖师黑框
		setlinecolor(BLACK);
		setlinestyle(PS_SOLID, 3);
		rectangle(WIDTH * 6 / 8 + 60 - 5, 140 - 5,
					WIDTH * 6 / 8 + 60 + AVATAR_SIZE + 5, 140 + AVATAR_SIZE + 5);
		settextcolor(BLACK);
		settextstyle(AVATAR_SIZE / 2, AVATAR_SIZE / 4, _T("Calibri"));
		outtextxy(WIDTH * 6 / 8 + 60 + AVATAR_SIZE / 2 - 10, 140 + AVATAR_SIZE / 4, _T("X"));
		if(rounds.auc_no != -1){
			//打印拍卖师信息
			printPlayer(WIDTH * 6 / 8 + 60, 140, rounds.auc_no, 0);
		}
	}
	else if(up >= 0){
		//打印玩家标题
		settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
		settextcolor(BLACK);
		outtextxy(WIDTH * 6 / 8 + 20, 60, _T("Player:"));
		//打印玩家黑框
		setlinecolor(BLACK);
		setlinestyle(PS_SOLID, 3);
		rectangle(WIDTH * 6 / 8 + 60 - 5, 140 - 5,
					WIDTH * 6 / 8 + 60 + AVATAR_SIZE + 5, 140 + AVATAR_SIZE + 5);
		//打印玩家信息
		printPlayer(WIDTH * 6 / 8 + 60, 140, up, 0);
	}
	//打印右下
	if(down == 1){	
		//打印货品标题
		settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
		settextcolor(BLACK);
		outtextxy(WIDTH * 6 / 8 + 20, 290, _T("Card:"));
		//打印货品黑框
		int temp_size = HEIGHT / 4;
		setlinecolor(BLACK);
		setlinestyle(PS_SOLID, 3);
		rectangle(WIDTH * 6 / 8 + 20 - 5, 350 - 5,
					WIDTH * 6 / 8 + 20 + temp_size + 5, 350 + temp_size * 3 / 2 + 5);
		settextcolor(BLACK);
		settextstyle(temp_size / 2, temp_size / 4, _T("Calibri"));
		outtextxy(WIDTH * 6 / 8 + 20 + temp_size / 2 - 20, 350 + temp_size / 4, _T("?"));
		if(rounds.card != NULL && rounds.card -> score > 1){
			//打印货品卡牌封面
			PutImageZoom(WIDTH * 6 / 8 + 20, 350, temp_size, temp_size * 3 / 2, &rounds.card -> cover_source);
		}
	}
	else if(down == 2){
		//打印收益率标题
		settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
		settextcolor(BLACK);
		outtextxy(WIDTH * 6 / 8 + 20, 280, _T("Yeild Rate:"));
		//打印当前最低价和玩家
		settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
		settextcolor(BLACK);
		if(rounds.auc_no != -1){
			//打印出价
			TCHAR sal[100];
			_itot(rounds.salary, sal, 10);
			_tcscat(sal, _T(" %"));
			outtextxy(WIDTH * 6 / 8 + 70, 370, sal);
		}
		else{
			//无最低价
			outtextxy(WIDTH * 6 / 8 + 70, 370, _T("?"));
			outtextxy(WIDTH * 6 / 8 + 70 + 100, 370, _T("%"));
		}
	}
	//打印左侧
	if(left == 1){	
		//打印桌子
		int radius = HEIGHT / 2 - 100;//圆桌半径
		int center_x = WIDTH * 3 / 8;//圆桌中心X坐标
		int center_y = HEIGHT / 2;//圆桌中心Y坐标
		transparentimage(&table, center_x - radius, center_y - radius);
	}
	else if(left == 2){
		//打印柜子
		putimage(0, 0, &closet);
	}
}
void printButton(int x,int y,int h,char *text,COLORREF color){//传入中心坐标和高度
	setlinecolor(BLACK);
	setfillcolor(color);
	setlinestyle(PS_SOLID, 1);
	TCHAR *t = CharToTCHAR(text);
	//文字尺度
	int th = h / 2;
	int tw = th / 2;
	//间隔尺度
	int gy = (h - th) / 2;
	int gx = gy * 2;
	//方框尺度
	int l = _tcslen(t);
	int w = l * tw + gx * 2;
	fillrectangle(x - w / 2, y - h / 2, x + w / 2, y + h / 2);
	settextstyle(h / 2, h / 4, _T("Calibri"));
	settextcolor(WHITE);
	int tx = x - l * tw / 2;
	int ty = y - h / 2 + gy;
	outtextxy(tx, ty, t);
	delete[] t;
}
int checkButton(int x,int y,int h,char *text,ExMessage msg){
	if(msg.lbutton){
		TCHAR *t = CharToTCHAR(text);
		//文字尺度
		int th = h / 2;
		int tw = th / 2;
		//间隔尺度
		int gy = (h - th) / 2;
		int gx = gy * 2;
		//方框尺度
		int w = _tcslen(t) * tw + gx * 2;
		if(msg.x >= x - w / 2 && msg.x <= x + w / 2 &&
			msg.y >= y - h / 2 && msg.y <= y + h / 2){
			delete[] t;
			return 1;
		}
	}
	return 0;
}
void printButton(int type, char *text = NULL){//1确认 2放弃 3继续
	//根据type确定按钮位置、颜色和文字（文字可改）
	int x,y,h;
	char t[50];
	COLORREF color;
	if(type == 1){
		x = WIDTH * 7 / 8;
		y = HEIGHT - 75;
		h = 50;
		strcpy(t, "Confirm");
		color = GREEN;
	}
	else if(type == 2){
		x = WIDTH * 7 / 8;
		y = HEIGHT - 75;
		h = 50;
		strcpy(t, "Quit");
		color = RED;
	}
	else if(type == 3){
		x = WIDTH / 2;
		y = HEIGHT - 75;
		h = 50;
		strcpy(t, "Continue");
		color = GREEN;
	}
	if(text != NULL){
		t[0] = '\0';//清空t
		strcpy(t, text);
	}
	printButton(x, y, h, t, color);
}
int checkButton(int type,ExMessage msg, char *text = NULL){//1确认 2放弃 3继续
	int x, y, h;
	char t[50];
	if(type == 1){
		x = WIDTH * 7 / 8;
		y = HEIGHT - 75;
		h = 50;
		strcpy(t, "Confirm");
	}
	else if(type == 2){
		x = WIDTH * 7 / 8;
		y = HEIGHT - 75;
		h = 50;
		strcpy(t, "Quit");
	}
	else if(type == 3){
		x = WIDTH / 2;
		y = HEIGHT - 75;
		h = 50;
		strcpy(t, "Continue");
	}
	if(text != NULL){
		t[0] = '\0';//清空t
		strcpy(t, text);
	}
	return checkButton(x, y, h, t, msg);
}

//游戏流程界面函数
int printElection(int player_no){//新版拍卖长选举界面
	int temp_salary = 0;
	int confirm_salary = 0;
	while(confirm_salary < 10 && confirm_salary > -1){	
		//打印背景
		printBackground(-1,2,1);
		//计算圆桌参数
		int radius = HEIGHT / 2 - 100;//圆桌半径
		int center_x = WIDTH * 3 / 8;//圆桌中心X坐标
		int center_y = HEIGHT / 2;//圆桌中心Y坐标
		//打印玩家
		double theta;
		theta = 2 * 3.1415926 / player_num;//非拍卖师玩家间隔角度
		for(int i = 0;i < player_num;i++){
			int x = center_x + radius * sin(theta * i) - AVATAR_SIZE / 2;
			int y = center_y - radius * cos(theta * i) - AVATAR_SIZE / 2;
			//显示其他玩家出价
			if(players[i].salary == 0){
				//未出价显示为“--”
				settextcolor(WHITE);
				settextstyle(AVATAR_SIZE / 3, AVATAR_SIZE / 6, _T("Calibri"));
				outtextxy(x + AVATAR_SIZE / 8, y - AVATAR_SIZE / 3, _T("--"));
				outtextxy(x + AVATAR_SIZE / 8 + 20, y - AVATAR_SIZE / 3, _T("%"));
			}
			else if(players[i].salary == -1){
				//放弃显示为“Quit”
				settextcolor(RED);
				settextstyle(AVATAR_SIZE / 3, AVATAR_SIZE / 6, _T("Calibri"));
				outtextxy(x + AVATAR_SIZE / 8, y - AVATAR_SIZE / 3, _T("Quit"));
			}
			else{
				//显示出价
				settextcolor(WHITE);
				settextstyle(AVATAR_SIZE / 3, AVATAR_SIZE / 6, _T("Calibri"));
				TCHAR salary_num[10];
				_itot(players[i].salary, salary_num, 10);
				outtextxy(x + AVATAR_SIZE / 8, y - AVATAR_SIZE / 3, salary_num);
				outtextxy(x + AVATAR_SIZE / 8 + 40, y - AVATAR_SIZE / 3, _T("%"));
			}
			//显示玩家
			if(i == player_no){//当前玩家
				printPlayer(x, y, i, 12);//高亮显示当前玩家及信息
				//显示控制条
				setlinecolor(WHITE);
				setlinestyle(PS_SOLID, 1);
				setfillcolor(WHITE);
				fillrectangle(center_x - AVATAR_SIZE, center_y - AVATAR_SIZE / 2, 
								center_x + AVATAR_SIZE / 2, center_y + AVATAR_SIZE / 2);
				//输入玩家出价
				settextstyle(AVATAR_SIZE / 2, AVATAR_SIZE / 4, _T("Calibri"));
				settextcolor(BLACK);
				TCHAR sal[100];
				_itot(temp_salary, sal, 10);
				outtextxy(center_x - AVATAR_SIZE + 10, center_y - 10, sal);
				outtextxy(center_x - AVATAR_SIZE + 70, center_y - 10, _T("%"));
				//打印上下三角
				setlinecolor(BLUE);
				setfillcolor(BLUE);
				POINT pts1[] = {{center_x + AVATAR_SIZE + 10, center_y - AVATAR_SIZE / 2},//顶点
								{center_x + AVATAR_SIZE, center_y - AVATAR_SIZE / 5},//左下点
								{center_x + AVATAR_SIZE + 20, center_y - AVATAR_SIZE / 5}};//右下点
				fillpolygon(pts1, 3);
				setlinecolor(RED);
				setfillcolor(RED);
				POINT pts2[] = {{center_x + AVATAR_SIZE + 10, center_y + AVATAR_SIZE / 2},//底点
								{center_x + AVATAR_SIZE, center_y + AVATAR_SIZE / 5},//左上点
								{center_x + AVATAR_SIZE + 20, center_y + AVATAR_SIZE / 5}};//右上点
				fillpolygon(pts2, 3);
				//打印确认键
				setlinecolor(BLACK);
				setfillcolor(GREEN);
				fillrectangle(center_x - AVATAR_SIZE, center_y + AVATAR_SIZE,
								center_x + AVATAR_SIZE , center_y + AVATAR_SIZE + 50);
				settextstyle(AVATAR_SIZE / 3, AVATAR_SIZE / 6, _T("Calibri"));
				settextcolor(WHITE);
				outtextxy(center_x - AVATAR_SIZE + 20, center_y + AVATAR_SIZE + 15, _T("Confirm"));
				//打印放弃键
				printButton(2);
			}
			else{
				printPlayer(x, y, i, 2);//显示其他玩家
			}
		}
		FlushBatchDraw();
		confirm_salary = electionAI(player_no);
		//检测鼠标点击
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){//鼠标左键点击
			int x = msg.x;
			int y = msg.y;
			if(x >= center_x - AVATAR_SIZE && x <= center_x + AVATAR_SIZE &&
				y >= center_y + AVATAR_SIZE && y <= center_y + AVATAR_SIZE + 50){
				//点击了确认键
				confirm_salary = temp_salary;
			}
			else if(checkButton(2, msg)){
				//点击了放弃键
				confirm_salary = -1;
			}
			else if(x >= center_x + AVATAR_SIZE && x <= center_x + AVATAR_SIZE + 20){
				if(y >= center_y - AVATAR_SIZE / 2 && y <= center_y - AVATAR_SIZE / 5){
					//点击上三角，出价加10%
					if(temp_salary <= 90){
						temp_salary += 10;
					}
				}
				else if(y >= center_y + AVATAR_SIZE / 5 && y <= center_y + AVATAR_SIZE / 2){
					//点击下三角，出价减10%
					if(temp_salary >= 10){
						temp_salary -= 10;
					}
				}
			}
		}
	}
	return confirm_salary;
}
int printCardSelect(){//显示选卡界面，返回选卡编号
	int selected_no = -1;
	int temp_select = -1;//当前选择卡牌编号
	//计算卡牌间距
	int gap_x = ((WIDTH - 5 * CARD_WIDTH) / 2) / 6;
	int gap_y = ((HEIGHT - 2 * CARD_HEIGHT)  * 3 / 4) / 3 - 20;
	while(selected_no == -1){
		//清屏
		char title[100] = "Card Selection";
		printBackground(-1,1,2,title);
		if(temp_select != -1){
			//打印卡牌封面
			PutImageZoom(WIDTH * 6 / 8 + 20, 350,200,300,&cards[temp_select].cover_source);
		}
		//显示卡牌选项
		for(int i = 0;i < 10;i++){
			//计算卡牌位置
			int x = gap_x + (i % 5) * (CARD_WIDTH + gap_x);//列
			int y = gap_y + (i / 5) * (CARD_HEIGHT + gap_y);//行
			if(i == temp_select){
				//高亮显示当前选中卡牌
				setlinecolor(YELLOW);
				setlinestyle(PS_DASH,2);
				rectangle(x - 2, y - 2, x + CARD_WIDTH + 2, y + CARD_HEIGHT + 2);
			}
			//显示卡牌封面
			putimage(x, y, &cards[i].cover);
			//打印隐秘输入提示
			if(cards[i].score == 1){
				settextcolor(WHITE);
				settextstyle(CARD_HEIGHT / 6,CARD_HEIGHT / 12,_T("Calibri"));
				//分配按键
				if(i == 0) {
					TCHAR tp[] = _T("Press: 'a'");
					outtextxy(x + CARD_WIDTH / 8, y + CARD_HEIGHT + CARD_HEIGHT / 8, tp);
				}
				else if(i == 1) {
					TCHAR tp[] = _T("Press: 's'");
					outtextxy(x + CARD_WIDTH / 8, y + CARD_HEIGHT + CARD_HEIGHT / 8, tp);
				}
				else if(i == 2) {
					TCHAR tp[] = _T("Press: 'd'");
					outtextxy(x + CARD_WIDTH / 8, y + CARD_HEIGHT + CARD_HEIGHT / 8, tp);
				}
				else if(i == 3) {
					TCHAR tp[] = _T("Press: 'f'");
					outtextxy(x + CARD_WIDTH / 8, y + CARD_HEIGHT + CARD_HEIGHT / 8, tp);
				}
				else if(i == 4) {
					TCHAR tp[] = _T("Press: 'g'");
					outtextxy(x + CARD_WIDTH / 8, y + CARD_HEIGHT + CARD_HEIGHT / 8, tp);
				}
			}
			//打印已售出遮罩
			if(cards[i].belong != -1){
				//该卡牌已被卖出，显示遮罩
				settextcolor(RGB(200,200,200));
				settextstyle(CARD_HEIGHT / 2,CARD_WIDTH / 4,_T("Calibri"));
				outtextxy(x + CARD_WIDTH / 8, y + CARD_HEIGHT / 4, _T("Sold"));
			}
		}
		//打印隐秘输入提示
		settextcolor(YELLOW);
		settextstyle(CARD_HEIGHT / 6,CARD_HEIGHT / 12,_T("Calibri"));
		outtextxy(gap_x + CARD_WIDTH / 8, gap_y + CARD_HEIGHT + CARD_HEIGHT * 3 / 8 - 10, _T("Make Sure Others Don't See If You're Choosing A 1-Scored Card!"));
		//打印确认键
		printButton(1);
		//刷新界面
		FlushBatchDraw();
		//AI选择
		selected_no = cardSelectAI();
		//检测鼠标位置和点击或键盘输入
		ExMessage msg = getmessage(EX_MOUSE | EX_KEY);
		if(msg.message == WM_KEYUP){//键盘输入·
			if(msg.vkcode == 'A' && cards[0].belong == -1){
				selected_no = 0;
			}
			else if(msg.vkcode == 'S' && cards[1].belong == -1){
				selected_no = 1;
			}
			else if(msg.vkcode == 'D' && cards[2].belong == -1){
				selected_no = 2;
			}
			else if(msg.vkcode == 'F' && cards[3].belong == -1){
				selected_no = 3;
			}
			else if(msg.vkcode == 'G' && cards[4].belong == -1){
				selected_no = 4;
			}
		}
		if(msg.lbutton){//鼠标左键点击
			int x = msg.x;
			int y = msg.y;
			//检查是否点击了确认键
			if(checkButton(1, msg)){
				if(temp_select != -1){
					if(cards[temp_select].score > 1){
						//确认选择卡牌
						selected_no = temp_select;
					}
				}
			}
			for(int i = 0;i < 10;i++){
				//计算卡牌位置
				int card_x = gap_x + (i % 5) * (CARD_WIDTH + gap_x);//列
				int card_y = gap_y + (i / 5) * (CARD_HEIGHT + gap_y);//行
				if(x >= card_x && x <= card_x + CARD_WIDTH &&
					y >= card_y && y <= card_y + CARD_HEIGHT){
					if(temp_select == i){
						//再次点击已选卡牌，取消选择
						temp_select = -1;
					}
					else{
						if(cards[i].belong == -1){
							//该卡牌未被选择，选中该卡牌
							temp_select = i;
						}
					}
				}
			}
		}
	}
	return selected_no;
}
int printDiceSelect(int player_no){//显示选骰界面，返回选骰面数
	int selected_size = 0;//当前选择骰子面数
	int gap_x = (WIDTH * 3 / 4 - 2 * DICE_SIZE) / 3;
	int gap_y = HEIGHT / 6;
	//清屏
	printBackground(player_no,1);
	//打印标题
	settextstyle(HEIGHT / 15,HEIGHT / 30,_T("Calibri"));
	settextcolor(WHITE);
	outtextxy(90, 30, _T("Dice Selection"));
	//打印提示
	settextcolor(YELLOW);
	settextstyle(HEIGHT / 30,HEIGHT / 60,_T("Calibri"));
	outtextxy(90, 120, _T("Make Sure Others Don't See Your Choice!"));
	//显示骰子选项
	settextstyle(HEIGHT / 30,HEIGHT / 60,_T("Calibri"));
	//判断是否加价
	int dice_prices[4];
	for(int i = 0;i < 4;i++){
		dice_prices[i] = dices[i].price;
		if(players[player_no].ex_auc == 1){
			//上一轮为拍卖长，加价1
			dice_prices[i] = dices[i].price + 1;
		}
	}
	for(int i = 0;i < 4;i++){	
		int x = gap_x + (i % 2) * (DICE_SIZE + gap_x);//列
		int y = gap_y + (i / 2) * (DICE_SIZE + gap_y);//行
		//打印展示台
		if(i == 0 || i == 2){
			transparentimage(&showcase1, x, y + DICE_SIZE / 2 - DICE_SIZE / 16);
		}
		else{
			transparentimage(&showcase2, x, y + DICE_SIZE / 2);
		}
		//显示骰子图片和信息
		transparentimage(&dices[i].image, x,y);
		//显示骰子名称
		TCHAR *dice_name = CharToTCHAR(dices[i].name);
		TCHAR dice_price[10];
		_itot(dice_prices[i], dice_price, 10);
		_tcscat(dice_price, _T("$"));
		settextcolor(BLACK);
		outtextxy(x + DICE_SIZE / 4, y + DICE_SIZE + 10, dice_name);
		//调整价格颜色
		if(players[player_no].block < dice_prices[i]){
			settextcolor(RED);
		}
		else{
			settextcolor(GREEN);
		}
		outtextxy(x + DICE_SIZE / 4 ,y + DICE_SIZE + 45, dice_price);
		delete[] dice_name;
		//提示按键
		settextcolor(BLUE);
		if(i == 0){
			outtextxy(x + DICE_SIZE / 4, y + DICE_SIZE + 70, _T("Press 'a'"));
		}
		else if(i == 1){
			outtextxy(x + DICE_SIZE / 4, y + DICE_SIZE + 70, _T("Press 's'"));
		}
		else if(i == 2){
			outtextxy(x + DICE_SIZE / 4, y + DICE_SIZE + 70, _T("Press 'd'"));
		}
		else if(i == 3){
			outtextxy(x + DICE_SIZE / 4, y + DICE_SIZE + 70, _T("Press 'f'"));
		}
	}
	//打印放弃键
	printButton(2);
	//刷新显示
	FlushBatchDraw();
	selected_size = diceSelectAI(player_no);
	//检测秘密输入
	while(selected_size == 0){
		ExMessage msg = getmessage(EX_KEY | EX_MOUSE);
		if(msg.lbutton){//鼠标左键点击
			int x = msg.x;
			int y = msg.y;
			//检查是否点击了放弃键
			if(checkButton(2, msg)){
				//放弃选择骰子
				selected_size = -1;
			}
		}
		if(msg.message == WM_KEYUP){
			if(msg.vkcode == 'A'){
				if(players[player_no].block >= dice_prices[0]){
					selected_size = 4;
				}
			}
			else if(msg.vkcode == 'S'){
				if(players[player_no].block >= dice_prices[1]){
					selected_size = 6;
				}
			}
			else if(msg.vkcode == 'D'){
				if(players[player_no].block >= dice_prices[2]){
					selected_size = 12;
				}
			}
			else if(msg.vkcode == 'F'){
				if(players[player_no].block >= dice_prices[3]){
					selected_size = 20;
				}
			}
		}
	}
	return selected_size;
}
int printDiceRoll(int player_no){//显示掷骰界面,返回掷骰结果
	int roll_result = 0;
	int radius = HEIGHT / 2 - 100;//圆桌半径
	int center_x = WIDTH * 3 / 8;//圆桌中心X坐标
	int center_y = HEIGHT / 2;//圆桌中心Y坐标
	//清屏
	printBackground(-1,1,1);
	//打印竞拍者
	double theta;
	int auc_player = 0;//本轮参与拍卖的人数
	for(int i = 0;i < player_num;i++){
		if(players[i].identity != 1 && players[i].dice != NULL && players[i].dice_roll != -1){//非拍卖师、有骰子（或平局时非最高者）
			auc_player++;
		}
	}
	if(auc_player <= 1){
		return -1;//无竞拍者或仅有一名竞拍者，直接返回-1结束掷骰环节
	}
	theta = 2 * 3.1415926 / auc_player;//非拍卖师玩家间隔角度
	for(int i = 0,j = 0;i < player_num;i++,j++){
		if(players[i].identity != 1 && players[i].dice != NULL && players[i].dice_roll != -1){//非拍卖师、有骰子（或平局时非最高者）
			//计算玩家位置
			int x = center_x + radius * sin(theta * j) - AVATAR_SIZE / 2;
			int y = center_y - radius * cos(theta * j) - AVATAR_SIZE / 2;
			if(players[i].dice_roll > 0){//显示掷骰结果
				settextcolor(RED);
				settextstyle(AVATAR_SIZE / 3, AVATAR_SIZE / 6, _T("Calibri"));
				TCHAR dice_num[10];
				_itot(players[i].dice_roll, dice_num, 10);
				outtextxy(x + AVATAR_SIZE / 8, y - AVATAR_SIZE / 3, dice_num);
				outtextxy(x + AVATAR_SIZE / 8 + 20, y - AVATAR_SIZE / 3, _T("points"));
			}
			//显示玩家
			if(i == player_no){
				printPlayer(x, y, i, 12);//高亮显示当前玩家及信息
			}
			else{
				printPlayer(x, y, i, 0);//显示其他玩家
			}
		}
		else{
			j--;//拍卖师或放弃者（或平局时非最高者）不计入位置计算
		}
	}
	//打印骰子
	transparentimage(&players[player_no].dice->image_small, center_x - DICE_SIZE / 3 - 40, center_y - DICE_SIZE / 6);//骰子图片
	//打印掷骰区域
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 1);
	setfillcolor(WHITE);
	fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
					center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
	settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
	settextcolor(BLACK);
	outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, _T("?"));
	//打印掷骰键 
	setlinecolor(BLACK);
	setfillcolor(GREEN);
	fillrectangle(center_x + 60, center_y - DICE_SIZE / 8,
					center_x + 60 + DICE_SIZE / 3, center_y + DICE_SIZE / 8);
	settextstyle(HEIGHT / 30,HEIGHT / 60,_T("Calibri"));
	settextcolor(WHITE);
	outtextxy(center_x + 60 + 15, center_y - DICE_SIZE / 8 + 15, _T("Roll"));
	//刷新显示
	FlushBatchDraw();
	//AI掷骰
	if(diceRollAI(player_no)){
		//摇骰子
		settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
		settextcolor(BLACK);
		TCHAR dice_num[10];
		for(int i = 0;i < 40;i++){
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 1);
			setfillcolor(WHITE);
			fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
						center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
			_itot((i % players[player_no].dice->size) + 1, dice_num, 10);
			outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, dice_num);
			FlushBatchDraw();
			Sleep(30);
		}
		roll_result = (rand() % players[player_no].dice->size) + 1;
		_itot(roll_result, dice_num, 10);
		fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
						center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
		outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, dice_num);
		FlushBatchDraw();
		Sleep(2000);//显示结果2秒
	}
	//检测输入
	while(!roll_result){
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){//按下鼠标左键
			int x = msg.x;
			int y = msg.y;
			if(x >= center_x + 60 && x <= center_x + 60 + DICE_SIZE / 3 &&
					y >= center_y - DICE_SIZE / 8 && y <= center_y + DICE_SIZE / 8){
				//摇骰子
				settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
				settextcolor(BLACK);
				TCHAR dice_num[10];
				for(int i = 0;i < 40;i++){
					setlinecolor(BLACK);
					setlinestyle(PS_SOLID, 1);
					setfillcolor(WHITE);
					fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
								center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
					_itot((i % players[player_no].dice->size) + 1, dice_num, 10);
					outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, dice_num);
					FlushBatchDraw();
					Sleep(30);
				}
				roll_result = (rand() % players[player_no].dice->size) + 1;
				_itot(roll_result, dice_num, 10);
				fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
								center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
				outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, dice_num);
				FlushBatchDraw();
				Sleep(2000);//显示结果2秒
			}
		}
	}
	return roll_result;
}	
void printSettleRound(int sal){//显示结算回合界面
	//清屏
	cleardevice();
	putimage(0, 0, &background);
	//打印标题
	settextstyle(HEIGHT / 10,HEIGHT / 20,_T("Calibri"));
	settextcolor(WHITE);
	outtextxy(WIDTH / 2 - 150, 50, _T("Round"));
	TCHAR round_no[10];
	_itot(rounds.no, round_no, 10);
	outtextxy(WIDTH / 2 + 100, 50, round_no);
	//打印玩家信息
	int gap = (WIDTH - player_num * AVATAR_SIZE) / (player_num + 1);
	for(int i = 0;i < player_num;i++){
		int x = gap + i * (AVATAR_SIZE + gap);
		int y = HEIGHT / 4;
		//显示玩家及信息
		printPlayer(x, y, i, 2);
		if(i == rounds.auc_no){
			//标记拍卖师
			settextstyle(AVATAR_SIZE / 4, AVATAR_SIZE / 8, _T("Calibri"));
			settextcolor(YELLOW);
			outtextxy(x, y - AVATAR_SIZE / 3, _T("Auctioneer"));
			//高亮显示拍卖师
			printPlayer(x, y, i, 12);
			//打印拍卖师工资
			settextcolor(GREEN);
			TCHAR salary[100];
			_itot(sal, salary, 10);
			outtextxy(x + 90, y - AVATAR_SIZE / 3, salary);
			TCHAR temp[200] = _T("(+ ");
			_tcscat(temp, salary);
			_tcscat(temp, _T(" $)"));
			outtextxy(x + 73, y - AVATAR_SIZE / 3, temp);

		}
		else if(rounds.winner_no == i){
			//标记赢家
			settextstyle(AVATAR_SIZE / 4, AVATAR_SIZE / 8, _T("Calibri"));
			settextcolor(RED);
			outtextxy(x, y - AVATAR_SIZE / 3, _T("Round Winner"));
			//红框显示赢家
			printPlayer(x, y, i, -12);
		}
		//打印卡牌
		CARD_NODE *p = players[i].card_stack;
		CARD_NODE *bot = NULL;
		int k = 0;//该玩家卡牌计数（用于计算位置）
		//找到卡牌链表底部
		while(p != NULL){
			bot = p;
			p = p -> next;
		}
		//从底部开始打印卡牌
		p = bot;
		while(p != NULL){
			//计算卡牌大小
			int card_w = CARD_WIDTH * 2 / 3;
			int card_h = card_w * 3 / 2;
			//计算卡牌位置
			int card_x = x - AVATAR_SIZE / 8 + k * card_w / 10;
			int card_y = y + AVATAR_SIZE * 2 + k * card_h / 4;
			//打印卡牌封面
			PutImageZoom(card_x, card_y, card_w, card_h, &p -> card -> cover_source);
			//打印技能
			if(p -> card -> effect != 0){
				settextstyle(card_h / 7, card_h / 14, _T("Calibri"));
				if(p -> card -> effect == 1){
					settextcolor(HSLtoRGB(220, 1, 0.5));//技能可生效
				}
				else if(p -> card -> effect == -1){
					settextcolor(HSLtoRGB(10, 1, 0.5));//技能条件达成一半
				}
				else if(p -> card -> effect == -2){
					settextcolor(HSLtoRGB(0, 0, 0.3));//技能失效
				}
				TCHAR *effect_text = CharToTCHAR(p -> card -> effect_name);
				outtextxy(card_x + card_w + 5, card_y + 5, effect_text);
				delete[] effect_text;
			}
			//向上移动到上一张卡牌
			p = p -> prev;
			k++;
		}
	}
	//打印继续键
	printButton(3);
	//输出画面
	FlushBatchDraw();
	while(1){
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){//左键点击
			if(checkButton(3, msg)){
				break;
			}
		}
	}
}
void printSettleGame(int *winner){//显示结算游戏界面
	//清屏
	cleardevice();
	putimage(0, 0, &background);
	//打印标题
	settextstyle(HEIGHT / 10,HEIGHT / 20,_T("Calibri"));
	settextcolor(WHITE);
	outtextxy(WIDTH / 2 - 150, 50, _T("Game Over"));
	//打印玩家信息
	int gap = (WIDTH - player_num * AVATAR_SIZE) / (player_num + 1);
	for(int i = 0;i < player_num;i++){
		int x = gap + i * (AVATAR_SIZE + gap);
		int y = HEIGHT / 4;
		//显示玩家及信息
		printPlayer(x, y, i, 2);
		if(winner[i]){
			//高亮显示赢家
			printPlayer(x, y, i, 12);
			//标记赢家
			settextstyle(AVATAR_SIZE / 4, AVATAR_SIZE / 8, _T("Calibri"));
			settextcolor(YELLOW);
			outtextxy(x, y - AVATAR_SIZE / 3, _T("Winner!"));
		}
		//打印卡牌
		for(int j = 0,k = 0;j < 10;j++){//j为卡牌编号（用于遍历），k为该玩家卡牌计数（用于计算位置）
			if(cards[j].belong == i){//该卡牌属于该玩家，堆叠打印卡牌封面
				//计算卡牌大小
				int card_w = CARD_WIDTH * 2 / 3;
				int card_h = card_w * 3 / 2;
				//计算卡牌位置
				int card_x = x - AVATAR_SIZE / 8 + k * card_w / 10;
				int card_y = y + AVATAR_SIZE * 2 + k * card_h / 4;
				//打印卡牌封面
				PutImageZoom(card_x, card_y, card_w, card_h, &cards[j].cover_source);
				k++;
			}
		}
	}
	//打印继续键
	char t[50] = "Exit";
	printButton(3, t);
	//输出画面
	FlushBatchDraw();
	while(1){
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){//左键点击
			if(checkButton(3, msg, t)){
				break;
			}
		}
	}
}
//技能界面函数
int printDuel(int player_no){//显示决斗选人界面，返回目标玩家编号
	int confirm_no = -1;//确定目标玩家编号
	int temp_no = -1;//当前选中目标玩家编号
	while(confirm_no == -1){
		//清屏
		cleardevice();
		putimage(0, 0, &background);
		//打印标题
		settextstyle(HEIGHT / 10,HEIGHT / 20,_T("Calibri"));
		settextcolor(WHITE);
		outtextxy(WIDTH / 2 - 150, 50, _T("Duel!"));
		//打印玩家信息
		int gap = (WIDTH - player_num * AVATAR_SIZE) / (player_num + 1);
		for(int i = 0;i < player_num;i++){
			int x = gap + i * (AVATAR_SIZE + gap);
			int y = HEIGHT / 4;
			//显示玩家及信息
			printPlayer(x, y, i, 2);
			if(i == player_no){
				//标记决斗发起者
				settextstyle(AVATAR_SIZE / 4, AVATAR_SIZE / 8, _T("Calibri"));
				settextcolor(YELLOW);
				outtextxy(x, y - AVATAR_SIZE / 3, _T("Duel Initiator"));
				printPlayer(x, y, i, 12);//高亮显示决斗发起者
			}
			else if(temp_no == i){
				//标记决斗目标
				settextstyle(AVATAR_SIZE / 4, AVATAR_SIZE / 8, _T("Calibri"));
				settextcolor(RED);
				outtextxy(x, y - AVATAR_SIZE / 3, _T("Duel Target"));
				printPlayer(x, y, i, -12);//红框显示决斗目标
			}
			//打印卡牌
			for(int j = 0,k = 0;j < 10;j++){//j为卡牌编号（用于遍历），k为该玩家卡牌计数（用于计算位置）
				if(cards[j].belong == i){//该卡牌属于该玩家，堆叠打印卡牌封面
					//计算卡牌大小
					int card_w = CARD_WIDTH * 2 / 3;
					int card_h = card_w * 3 / 2;
					//计算卡牌位置
					int card_x = x - AVATAR_SIZE / 8 + k * card_w / 10;
					int card_y = y + AVATAR_SIZE * 2 + k * card_h / 4;
					//打印卡牌封面
					PutImageZoom(card_x, card_y, card_w, card_h, &cards[j].cover_source);
					k++;
				}
			}
		}
		//打印确认键
		int bx_1 = WIDTH / 2;
		int by_1 = HEIGHT - 65 - 70;
		char bt_1[50] = "Confirm";
		printButton(bx_1, by_1, 50, bt_1, GREEN);
		//打印放弃键
		int bx_2 = WIDTH / 2;
		int by_2 = HEIGHT - 65;
		char bt_2[50] = "Skip";
		printButton(bx_2, by_2, 50, bt_2, RED);
		//输出画面
		FlushBatchDraw();
		//AI选择
		confirm_no = duelAI(player_no);
		//检测输入
		ExMessage msg;
		peekmessage(&msg, EX_MOUSE);
		if(msg.lbutton){//左键点击
			//检查是否点击了放弃键
			if(checkButton(bx_2, by_2, 50, bt_2, msg)){
				return -1;//放弃决斗，返回-1
			}
			//检查是否点击了确认键
			else if(checkButton(bx_1, by_1, 50, bt_1, msg)){
				if(temp_no != -1){
					confirm_no = temp_no;
				}
			}
			//检查是否点击了玩家头像
			for(int i = 0;i < player_num;i++){
				if(i != player_no){
					//数卡牌数
					int card_count = 0;
					for(int j = 0;j < 10;j++){
						if(cards[j].belong == i){
							card_count++;
						}
					}
					if(card_count > 0){//该玩家有卡牌，可以选择
						int x = gap + i * (AVATAR_SIZE + gap);
						int y = HEIGHT / 4;
						if(msg.x >= x && msg.x <= x + AVATAR_SIZE &&
								msg.y >= y && msg.y <= y + AVATAR_SIZE){
							if(temp_no == i){
								//再次点击已选玩家，取消选择
								temp_no = -1;
								break;
							}
							else{
								//选中该玩家
								temp_no = i;
								break;
							}
						}
					}
				}
			}
		}
	}
	return confirm_no;
}
int printDuelCard(int player_no,int target_no){//显示决斗卡牌选择界面
	int selected_no = -1;//确定选择卡牌编号
	int temp_select = -1;//当前选中卡牌编号
	while(selected_no == -1){
		//清屏
		cleardevice();
		putimage(0, 0, &background);
		//计算位置
		int x = 100;
		int y1 = HEIGHT / 4;
		int y2 = HEIGHT * 5 / 8;
		//打印标题
		settextstyle(HEIGHT / 10,HEIGHT / 20,_T("Calibri"));
		settextcolor(WHITE);
		outtextxy(WIDTH / 4, 50, _T("Duel Card Selection"));
		settextcolor(RED);
		outtextxy(x,(y1 + y2) / 2 + 10, _T("VS"));
		//打印玩家信息
		printPlayer(x, y1, player_no, 2);
		printPlayer(x, y2, target_no, 2);
		//打印卡牌
		for(int i = 0,j = 0,k = 0;i < 10;i++){//i为卡牌编号（用于遍历），j为玩家1卡牌计数，k为玩家2卡牌计数（用于计算位置）
			if(cards[i].belong == player_no){
				//计算卡牌位置
				int card_x = x + AVATAR_SIZE + j * (CARD_WIDTH + 20) + 50;
				int card_y = y1;
				//显示卡牌封面
				putimage(card_x, card_y, &cards[i].cover);
				j++;
			}
			else if(cards[i].belong == target_no){
				//计算卡牌位置
				int card_x = x + AVATAR_SIZE + k * (CARD_WIDTH + 20) + 50;
				int card_y = y2;
				if(i == temp_select){
					//高亮显示当前选中卡牌
					setlinecolor(YELLOW);
					setlinestyle(PS_DASH,2);
					rectangle(card_x - 2, card_y - 2, card_x + CARD_WIDTH + 2, card_y + CARD_HEIGHT + 2);
				}
				//显示卡牌封面
				putimage(card_x, card_y, &cards[i].cover);
				k++;
			}
		}
		//打印确认键
		char t[50] = "Confirm";
		printButton(3,t);
		//输出画面
		FlushBatchDraw();
		//AI选择
		selected_no = duelCardAI(player_no, target_no);
		//检测输入
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){//左键点击
			//检查是否点击了确认键
			if(checkButton(3, msg, t)){
				if(temp_select != -1){
					selected_no = temp_select;
					break;
				}
			}
			//检查是否点击了目标玩家的卡牌
			for(int i = 0,k = 0;i < 10;i++){//i为卡牌编号（用于遍历），k为目标玩家卡牌计数（用于计算位置）
				if(cards[i].belong == target_no){
					int card_x = x + AVATAR_SIZE + k * (CARD_WIDTH + 20) + 50;
					int card_y = y2;
					if(msg.x >= card_x && msg.x <= card_x + CARD_WIDTH &&
						 msg.y >= card_y && msg.y <= card_y + CARD_HEIGHT){
						if(temp_select == i){
							//再次点击已选卡牌，取消选择
							temp_select = -1;
						}
						else{
							//选中该卡牌
							temp_select = i;
						}
					}
					k++;
				}
			}
		}
	}
	return selected_no;
}
int printFortune(int player_no){
	int roll_result = 0;
	//清屏
	printBackground(-1,1,1);
	//计算位置
	int radius = HEIGHT / 2 - 100;//圆桌半径
	int center_x = WIDTH * 3 / 8;//圆桌中心X坐标
	int center_y = HEIGHT / 2;//圆桌中心Y坐标
	//打印竞拍者
	double theta;
	int auc_player = 0;//本轮参与拍卖的人数
	for(int i = 0;i < player_num;i++){
		if(players[i].identity != 1 && players[i].dice != NULL && players[i].dice_roll != -1){//非拍卖师、有骰子（或平局时非最高者）
			auc_player++;
		}
	}
	if(auc_player <= 1){
		return -1;//无竞拍者或仅有一名竞拍者，直接返回-1结束掷骰环节
	}
	theta = 2 * 3.1415926 / auc_player;//非拍卖师玩家间隔角度
	for(int i = 0,j = 0;i < player_num;i++,j++){
		if(players[i].identity != 1 && players[i].dice != NULL && players[i].dice_roll != -1){//非拍卖师、有骰子（或平局时非最高者）
			//计算玩家位置
			int x = center_x + radius * sin(theta * j) - AVATAR_SIZE / 2;
			int y = center_y - radius * cos(theta * j) - AVATAR_SIZE / 2;
			if(players[i].dice_roll > 0){//显示掷骰结果
				settextcolor(RED);
				settextstyle(AVATAR_SIZE / 3, AVATAR_SIZE / 6, _T("Calibri"));
				TCHAR dice_num[10];
				_itot(players[i].dice_roll, dice_num, 10);
				outtextxy(x + AVATAR_SIZE / 8, y - AVATAR_SIZE / 3, dice_num);
				outtextxy(x + AVATAR_SIZE / 8 + 20, y - AVATAR_SIZE / 3, _T("points"));
			}
			//显示玩家
			if(i == player_no){
				printPlayer(x, y, i, 12);//高亮显示当前玩家及信息
			}
			else{
				printPlayer(x, y, i, 0);//显示其他玩家
			}
		}
		else{
			j--;//拍卖师或放弃者（或平局时非最高者）不计入位置计算
		}
	}
	//打印骰子
	transparentimage(&players[player_no].dice->image_small, center_x - DICE_SIZE / 3 - 40, center_y - DICE_SIZE / 6);//骰子图片
	//打印掷骰区域
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 1);
	setfillcolor(WHITE);
	fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
					center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
	settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
	settextcolor(BLACK);
	outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, _T("?"));
	//打印掷骰键 
	setlinecolor(BLACK);
	setfillcolor(GREEN);
	fillrectangle(center_x + 60, center_y - DICE_SIZE / 8,
					center_x + 60 + DICE_SIZE * 2 / 3, center_y + DICE_SIZE / 8);
	settextstyle(HEIGHT / 30,HEIGHT / 60,_T("Calibri"));
	settextcolor(WHITE);
	outtextxy(center_x + 60 + 15, center_y - DICE_SIZE / 8 + 15, _T("Roll Again"));
	//打印放弃键
	setlinecolor(BLACK);
	setfillcolor(RED);
	fillrectangle(center_x + 60, center_y + DICE_SIZE / 8 + 10,
					center_x + 60 + DICE_SIZE / 3, center_y + DICE_SIZE / 8 + 50);
	settextstyle(HEIGHT / 30, HEIGHT / 60, _T("Calibri"));
	settextcolor(WHITE);
	outtextxy(center_x + 60 + 15, center_y + DICE_SIZE / 8 + 20, _T("Skip"));
	//打印技能
	settextstyle(HEIGHT / 20, HEIGHT / 40, _T("Calibri"));
	settextcolor(HSLtoRGB(220, 1, 0.5));//技能可生效
	outtextxy(center_x - 50, center_y - DICE_SIZE / 3 - 60, _T("Fortune"));
	settextcolor(YELLOW);
	settextstyle(HEIGHT / 30, HEIGHT / 60, _T("Calibri"));
	outtextxy(center_x - 180, center_y - DICE_SIZE / 3 - 10, _T("Roll again and keep the better result"));
	//刷新显示
	FlushBatchDraw();
	//AI选择
	if(fortuneAI(player_no)){
		//摇骰子
		settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
		settextcolor(BLACK);
		TCHAR dice_num[10];
		for(int i = 0;i < 40;i++){
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 1);
			setfillcolor(WHITE);
			fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
						center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
			_itot((i % players[player_no].dice->size) + 1, dice_num, 10);
			outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, dice_num);
			FlushBatchDraw();
			Sleep(30);
		}
		roll_result = (rand() % players[player_no].dice->size) + 1;
		_itot(roll_result, dice_num, 10);
		fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
						center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
		outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, dice_num);
		FlushBatchDraw();
		Sleep(2000);//显示结果2秒
	}
	//检测输入
	while(!roll_result){
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){//按下鼠标左键
			int x = msg.x;
			int y = msg.y;
			if(x >= center_x + 60 && x <= center_x + 60 + DICE_SIZE * 2 / 3 &&
					y >= center_y - DICE_SIZE / 8 && y <= center_y + DICE_SIZE / 8){
				//摇骰子
				settextstyle(HEIGHT / 20,HEIGHT / 40,_T("Calibri"));
				settextcolor(BLACK);
				TCHAR dice_num[10];
				for(int i = 0;i < 40;i++){
					setlinecolor(BLACK);
					setlinestyle(PS_SOLID, 1);
					setfillcolor(WHITE);
					fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
								center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
					_itot((i % players[player_no].dice->size) + 1, dice_num, 10);
					outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, dice_num);
					FlushBatchDraw();
					Sleep(30);
				}
				roll_result = (rand() % players[player_no].dice->size) + 1;
				_itot(roll_result, dice_num, 10);
				fillrectangle(center_x - 30, center_y - DICE_SIZE / 6,
								center_x - 30 + DICE_SIZE / 3, center_y + DICE_SIZE / 6);
				outtextxy(center_x - 30 + 20, center_y - DICE_SIZE / 6 + 15, dice_num);
				FlushBatchDraw();
				Sleep(2000);//显示结果2秒
			}
			else if(x >= center_x + 60 && x <= center_x + 60 + DICE_SIZE / 3 &&
					y >= center_y + DICE_SIZE / 8 + 10 && y <= center_y + DICE_SIZE / 8 + 50){
				//放弃使用技能
				roll_result = -1;
			}
		}
	}
	return roll_result;
}
int printPriceless(int player_no){
	//清屏
	printBackground(-1,1,1);
	//计算位置
	int radius = HEIGHT / 2 - 100;//圆桌半径
	int center_x = WIDTH * 3 / 8;//圆桌中心X坐标
	int center_y = HEIGHT / 2;//圆桌中心Y坐标
	//打印竞拍者
	double theta;
	int auc_player = 0;//本轮参与拍卖的人数
	for(int i = 0;i < player_num;i++){
		if(players[i].identity != 1 && players[i].dice != NULL && players[i].dice_roll != -1){//非拍卖师、有骰子（或平局时非最高者）
			auc_player++;
		}
	}
	if(auc_player <= 1){
		return -1;//无竞拍者或仅有一名竞拍者，直接返回-1结束掷骰环节
	}
	theta = 2 * 3.1415926 / auc_player;//非拍卖师玩家间隔角度
	for(int i = 0,j = 0;i < player_num;i++,j++){
		if(players[i].identity != 1 && players[i].dice != NULL && players[i].dice_roll != -1){//非拍卖师、有骰子（或平局时非最高者）
			//计算玩家位置
			int x = center_x + radius * sin(theta * j) - AVATAR_SIZE / 2;
			int y = center_y - radius * cos(theta * j) - AVATAR_SIZE / 2;
			if(players[i].dice_roll > 0){//显示掷骰结果
				settextcolor(RED);
				settextstyle(AVATAR_SIZE / 3, AVATAR_SIZE / 6, _T("Calibri"));
				TCHAR dice_num[10];
				_itot(players[i].dice_roll, dice_num, 10);
				outtextxy(x + AVATAR_SIZE / 8, y - AVATAR_SIZE / 3, dice_num);
				outtextxy(x + AVATAR_SIZE / 8 + 20, y - AVATAR_SIZE / 3, _T("points"));
			}
			//显示玩家
			if(i == player_no){
				printPlayer(x, y, i, 12);//高亮显示当前玩家及信息
			}
			else{
				printPlayer(x, y, i, 0);//显示其他玩家
			}
		}
		else{
			j--;//拍卖师或放弃者（或平局时非最高者）不计入位置计算
		}
	}
	//打印骰子
	transparentimage(&players[player_no].dice->image_small,
					 center_x - DICE_SIZE / 6, center_y - DICE_SIZE / 6);
	//打印确认键
	setlinestyle(PS_SOLID, 1);
	setlinecolor(BLACK);
	setfillcolor(GREEN);
	fillrectangle(center_x + 60, center_y - DICE_SIZE / 8,
					center_x + 60 + DICE_SIZE * 2 / 3, center_y + DICE_SIZE / 8);
	settextstyle(HEIGHT / 30,HEIGHT / 60,_T("Calibri"));
	settextcolor(WHITE);
	outtextxy(center_x + 60 + 15, center_y - DICE_SIZE / 8 + 15, _T("Upgrade"));
	//打印放弃键
	setlinecolor(BLACK);
	setfillcolor(RED);
	fillrectangle(center_x + 60, center_y + DICE_SIZE / 8 + 10,
					center_x + 60 + DICE_SIZE / 3, center_y + DICE_SIZE / 8 + 50);
	settextstyle(HEIGHT / 30, HEIGHT / 60, _T("Calibri"));
	settextcolor(WHITE);
	outtextxy(center_x + 60 + 15, center_y + DICE_SIZE / 8 + 20, _T("Skip"));
	//打印技能
	settextstyle(HEIGHT / 20, HEIGHT / 40, _T("Calibri"));
	settextcolor(HSLtoRGB(220, 1, 0.5));//技能可生效
	outtextxy(center_x - 50, center_y - DICE_SIZE / 3 - 60, _T("Priceless"));
	settextcolor(YELLOW);
	settextstyle(HEIGHT / 30, HEIGHT / 60, _T("Calibri"));
	outtextxy(center_x - 180, center_y - DICE_SIZE / 3 - 10, _T("Upgrade your dice (one time only)"));
	//刷新显示
	FlushBatchDraw();
	//检测输入
	while(1){
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){//按下鼠标左键
			int x = msg.x;
			int y = msg.y;
			if(x >= center_x + 60 && x <= center_x + 60 + DICE_SIZE * 2 / 3 &&
					y >= center_y - DICE_SIZE / 8 && y <= center_y + DICE_SIZE / 8){
				//使用技能
				return 1;
			}
			else if(x >= center_x + 60 && x <= center_x + 60 + DICE_SIZE / 3 &&
					y >= center_y + DICE_SIZE / 8 + 10 && y <= center_y + DICE_SIZE / 8 + 50){
				//放弃使用技能
				return 0;
			}
		}
	}
}
int printAcumen(int salary){
	//清屏
	cleardevice();
	putimage(0, 0, &background);
	//打印技能
	settextstyle(HEIGHT / 10,HEIGHT / 20,_T("Calibri"));
	settextcolor(HSLtoRGB(220, 1, 0.5));//技能可生效
	outtextxy(WIDTH / 2 - 150, 30, _T("Priceless"));
	settextcolor(YELLOW);
	settextstyle(HEIGHT / 30, HEIGHT / 60, _T("Calibri"));
	outtextxy(WIDTH / 2 - 250, 120, _T("Auctioneer can double the salary (one time only)"));
	//打印玩家信息
	int gap = (WIDTH - player_num * AVATAR_SIZE) / (player_num + 1);
	for(int i = 0;i < player_num;i++){
		int x = gap + i * (AVATAR_SIZE + gap);
		int y = HEIGHT / 4;
		//显示玩家及信息
		printPlayer(x, y, i, 2);
		if(i == rounds.auc_no){
			//标记拍卖师
			settextstyle(AVATAR_SIZE / 4, AVATAR_SIZE / 8, _T("Calibri"));
			settextcolor(YELLOW);
			outtextxy(x, y - AVATAR_SIZE / 3, _T("Auctioneer"));
			//高亮显示拍卖师
			printPlayer(x, y, i, 12);
			//打印拍卖师工资
			settextcolor(GREEN);
			TCHAR sal[100];
			_itot(salary, sal, 10);
			outtextxy(x + 90, y - AVATAR_SIZE / 3, sal);
			if(salary < 10){
				outtextxy(x + 73, y - AVATAR_SIZE / 3, _T("(+      $)"));
			}
			else{
				outtextxy(x + 73, y - AVATAR_SIZE / 3, _T("(+        $)"));
			}
		}
		else if(rounds.winner_no == i){
			//标记赢家
			settextstyle(AVATAR_SIZE / 4, AVATAR_SIZE / 8, _T("Calibri"));
			settextcolor(RED);
			outtextxy(x, y - AVATAR_SIZE / 3, _T("Round Winner"));
			//红框显示赢家
			printPlayer(x, y, i, -12);
		}
		//打印卡牌
		for(int j = 0,k = 0;j < 10;j++){//j为卡牌编号（用于遍历），k为该玩家卡牌计数（用于计算位置）
			if(cards[j].belong == i){//该卡牌属于该玩家，堆叠打印卡牌封面
				//计算卡牌大小
				int card_w = CARD_WIDTH * 2 / 3;
				int card_h = card_w * 3 / 2;
				//计算卡牌位置
				int card_x = x - AVATAR_SIZE / 8 + k * card_w / 10;
				int card_y = y + AVATAR_SIZE * 2 + k * card_h / 4;
				//打印卡牌封面
				PutImageZoom(card_x, card_y, card_w, card_h, &cards[j].cover_source);
				//打印技能
				if(cards[j].effect != 0){
					settextstyle(card_h / 7, card_h / 14, _T("Calibri"));
					if(cards[j].effect == 1){
						settextcolor(HSLtoRGB(220, 1, 0.5));//技能可生效
					}
					else if(cards[j].effect == -1){
						settextcolor(HSLtoRGB(10, 1, 0.5));//技能条件达成一半
					}
					else if(cards[j].effect == -2){
						settextcolor(HSLtoRGB(0, 0, 0.3));//技能失效
					}
					TCHAR *effect_text = CharToTCHAR(cards[j].effect_name);
					outtextxy(card_x + card_w + 5, card_y + 5, effect_text);
					delete[] effect_text;
				}
				k++;
			}
		}
	}
	//打印放弃键
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 1);
	setfillcolor(RED);
	fillrectangle(WIDTH / 2 - 60, HEIGHT - 100,
					WIDTH / 2 + 60, HEIGHT - 50);
	settextstyle(HEIGHT / 30,HEIGHT / 60,_T("Calibri"));
	settextcolor(WHITE);
	outtextxy(WIDTH / 2 - 40, HEIGHT - 90, _T("Skip"));
	//打印确认键
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 1);
	setfillcolor(GREEN);
	fillrectangle(WIDTH / 2 - 60, HEIGHT - 180,
					WIDTH / 2 + 60, HEIGHT - 130);
	settextstyle(HEIGHT / 30,HEIGHT / 60,_T("Calibri"));
	settextcolor(WHITE);
	outtextxy(WIDTH / 2 - 40, HEIGHT - 170, _T("Double"));
	//输出画面
	FlushBatchDraw();
	while(1){
		ExMessage msg = getmessage(EX_MOUSE);
		if(msg.lbutton){//左键点击
			if(msg.x > WIDTH / 2 - 50 && msg.x < WIDTH / 2 + 50 &&
				 msg.y > HEIGHT - 100 && msg.y < HEIGHT - 50){
				//放弃使用技能
				return 0;
			}
			else if(msg.x > WIDTH / 2 - 50 && msg.x < WIDTH / 2 + 50 &&
					 msg.y > HEIGHT - 180 && msg.y < HEIGHT - 130){
				//使用技能
				return 1;
			}
		}
	}
}
