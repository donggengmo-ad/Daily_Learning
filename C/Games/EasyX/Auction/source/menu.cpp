#include "auction.h"

#define MENU_WIDTH 800
#define MENU_HEIGHT 430

typedef struct _CHAR_NODE{
    CHARACTER *character;
    struct _CHAR_NODE *next;
} CHAR_NODE;//角色链表节点结构体

int player_num;
int AI[8];//记录玩家是否为AI，1是0否
int character_choose[8];//记录玩家选择角色编号



void outtextxywrap(int x, int y, const TCHAR* str, int lineHeight) {// 自动解析 TCHAR 字符串中的 \n，实现换行输出
    // 参数：x/y - 起始坐标；str - 含 \n 的 TCHAR 字符串；lineHeight - 行高（建议字号+10）
    if (str == NULL || lineHeight <= 0) return;

    TCHAR buf[1024] = {0}; // 临时缓冲区，存储单行内容
    const TCHAR* p = str;  // 遍历字符串的指针
    int bufIdx = 0;        // 缓冲区索引

    while (*p != _T('\0')) { // 遍历到字符串结束
        if (*p == _T('\n')) { // 遇到换行符，输出当前行
            buf[bufIdx] = _T('\0'); // 终止单行字符串
            outtextxy(x, y, buf);
            y += lineHeight;        // 换行：Y 坐标增加
            bufIdx = 0;             // 重置缓冲区索引
        } else { // 非换行符，存入缓冲区
            buf[bufIdx++] = *p;
        }
        p++;
    }

    // 输出最后一行（无 \n 结尾的剩余内容）
    if (bufIdx > 0) {
        buf[bufIdx] = _T('\0');
        outtextxy(x, y, buf);
    }
}
void printCharacter(int x,int y,CHARACTER *cha,int mode){//mode 0普通,1高亮带信息,-1红框，-2红框不可选
	int size = AVATAR_SIZE / 4;
	//显示玩家名称
	settextstyle(size,size / 2,_T("Calibri"));
	settextcolor(BLACK);
	TCHAR *cha_name = CharToTCHAR(cha -> name);
	outtextxy(x,y + AVATAR_SIZE + size / 2, cha_name);
	delete[] cha_name;
	//显示玩家头像
	transparentimage(&cha -> avatar,x, y);
	if(mode == 1){  
		//显示高亮框
		setlinecolor(YELLOW);
		setlinestyle(PS_DASH,2);
		rectangle(x - 2, y - 2, x + AVATAR_SIZE + 2, y + AVATAR_SIZE + 2);
        //显示角色描述
        settextcolor(WHITE);
        settextstyle(size,size / 2,_T("Calibri"));
        TCHAR *cha_des = CharToTCHAR(cha -> description);
        outtextxywrap(MENU_WIDTH / 4, y + AVATAR_SIZE + 40, cha_des, size);
        delete[] cha_des;
	}
    else if(mode == -1){  
		//显示红框
		setlinecolor(RED);
		setlinestyle(PS_DASH,2);
		rectangle(x - 2, y - 2, x + AVATAR_SIZE + 2, y + AVATAR_SIZE + 2);
	}
    else if(mode == -2){  
		//显示红框
		setlinecolor(RED);
		setlinestyle(PS_DASH,2);
		rectangle(x - 2, y - 2, x + AVATAR_SIZE + 2, y + AVATAR_SIZE + 2);
        //显示角色描述
        settextcolor(WHITE);
        settextstyle(size,size / 2,_T("Calibri"));
        outtextxy(MENU_WIDTH / 4, y + AVATAR_SIZE + 40, _T("This character has been chosen by other player, please select another one."));
	}
}

void numMenu(){//选择玩家人数界面
    cleardevice();
    rounds.state = 0;//设置回合状态为菜单
    //打印标题
    settextstyle(80,40,_T("Calibri"));
    TCHAR title[] = _T("Aution Game");
    outtextxy(180,70,title);
    //打印提示
    settextstyle(20,10,_T("Calibri"));
    TCHAR reminder[] = _T("Please Select The Number Of Players");
    outtextxy(220,200,reminder);
    //打印人数选项
    settextstyle(40,20,_T("Calibri"));
    for(int i = 0;i < 6;i++){
        TCHAR num[] = {0};
        _itot(i + 3,num,10);//int转TCHAR
        outtextxy(110 * (i + 1),300,num);
    }
    //刷新界面
    FlushBatchDraw();
    //等待人数选择
    ExMessage msg;
    while(1){
        msg = getmessage(EX_MOUSE);
        if(msg.lbutton){
            if(msg.y <= 350 && msg.y >= 300){
                for(int i = 0;i < 6;i++){
                    if(msg.x >= (i + 1) * 110 && msg.x <= (i + 2) * 110){
                        player_num = i + 3;
                        return;
                    }
                }
            }
        }
    }
}
void AIMenu(int player_no){//选择玩家是否为AI界面
    //清屏
    cleardevice();
    //打印标题
    settextstyle(80,40,_T("Calibri"));
    outtextxy(180,70,_T("Aution Game"));
    //打印提示
    settextstyle(20,10,_T("Calibri"));
    TCHAR reminder[200] = _T("Is Player ");
    TCHAR num[10];
    _itot(player_no + 1,num,10);
    _tcscat(reminder,num);
    _tcscat(reminder,_T(" an AI?"));
    outtextxy(220,200,reminder);
    //打印选项
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID, 1);
    setfillcolor(GREEN);
    fillrectangle(200,300,350,350);
    setfillcolor(RED);
    fillrectangle(450,300,600,350);
    settextstyle(40,20,_T("Calibri"));
    settextcolor(WHITE);
    outtextxy(230,310,_T("AI"));
    outtextxy(480,310,_T("Human"));
    //刷新界面
    FlushBatchDraw();
    //等待选择
    ExMessage msg;
    while(1){
        msg = getmessage(EX_MOUSE);
        if(msg.lbutton){
            if(msg.y <= 350 && msg.y >= 300){
                if(msg.x >= 200 && msg.x <= 350){
                    AI[player_no] = 1;
                    return;
                }
                else if(msg.x >= 450 && msg.x <= 600){
                    AI[player_no] = 0;
                    return;
                }
            }
        }
    }

}
void characterMenu(int player_no){//选择玩家角色界面
    //创建角色循环链表
    CHAR_NODE *head = NULL;
    CHAR_NODE *tail = NULL;
    for(int i = 0;i < 8;i++){
        //如果是第一个角色，创建头节点
        if(head == NULL){
            head = (CHAR_NODE *)malloc(sizeof(CHAR_NODE));
            head -> character = &characters[i];
            head -> next = NULL;
            tail = head;
        } 
        //如果不是第一个角色，头插法
        else{
            CHAR_NODE *new_p = (CHAR_NODE *)malloc(sizeof(CHAR_NODE));
            new_p -> character = &characters[i];
            new_p -> next = head;
            head = new_p;
        }
    }
    //形成循环链表
    tail -> next = head;
    while(1){
        //清屏
        cleardevice();
        //打印提示
        settextstyle(30,15,_T("Calibri"));
        outtextxy(20,20,_T("Please Select Your Character"));
        //打印角色选项
        int gap = (MENU_WIDTH - 5 * AVATAR_SIZE) / 6;//计算角色之间的间距
        CHAR_NODE *p = head;
        CHAR_NODE *temp = head;
        for(int i = 0;i < 5;i++){//显示5个角色
            int x = gap + i * (AVATAR_SIZE + gap);
            int y = MENU_HEIGHT / 4;
            printCharacter(x, y, p->character, 0);
            if(i == 2 && !p->character->chosen){//如果是中间角色，显示提示
                printCharacter(x, y, p->character, 1);
                temp = p;
            }
            else if(i == 2 && p->character->chosen){//如果是中间角色但已被选择，显示提示和红框
                printCharacter(x, y, p->character, -2);
                temp = p;
            }
            else if(p->character->chosen){//如果角色已被选择，显示红框
                printCharacter(x, y, p->character, -1);
            }
            p = p->next;
        }
        //打印确认键
        setlinecolor(BLACK);
        setfillcolor(GREEN);
        setlinestyle(PS_SOLID, 1);
        fillrectangle(MENU_WIDTH / 2 - 50, MENU_HEIGHT - 100,
                        MENU_WIDTH / 2 + 50, MENU_HEIGHT - 50);
        settextstyle(HEIGHT / 30,HEIGHT / 60,_T("Calibri"));
        settextcolor(WHITE);
        outtextxy(MENU_WIDTH / 2 - 40, MENU_HEIGHT - 90, _T("Confirm"));
        //打印左右切换键
        setlinecolor(BLACK);
        setfillcolor(WHITE);
        POINT left[3] = {{MENU_WIDTH / 2 - 50 - 50, MENU_HEIGHT - 75},//左顶点
                        {MENU_WIDTH / 2 - 50 - 20, MENU_HEIGHT - 60},//右下点
                        {MENU_WIDTH / 2 - 50 - 20, MENU_HEIGHT - 90}};//右上点
        POINT right[3] = {{MENU_WIDTH / 2 + 50 + 50, MENU_HEIGHT - 75},//右顶点
                        {MENU_WIDTH / 2 + 50 + 20, MENU_HEIGHT - 60},//左下点
                        {MENU_WIDTH / 2 + 50 + 20, MENU_HEIGHT - 90}};//左上点
        fillpolygon(left, 3);
        fillpolygon(right, 3);
        //刷新界面
        FlushBatchDraw();
        //等待角色选择
        ExMessage msg;
        while(1){
            msg = getmessage(EX_MOUSE);
            if(msg.lbutton){
                int x = msg.x;
                int y = msg.y;
                if(x >= MENU_WIDTH / 2 - 50 && x <= MENU_WIDTH / 2 + 50 &&
                    y >= MENU_HEIGHT - 100 && y <= MENU_HEIGHT - 50){
                    //点击了确认键
                    if(temp->character->chosen == 0){//如果该角色未被选择
                        character_choose[player_no] = (temp->character - characters);//记录玩家选择的角色编号
                        temp->character->chosen = 1;//标记角色为已选择
                        return;
                    }
                    else{
                        continue;;
                    }
                }
                else if(x >= MENU_WIDTH / 2 - 100 && x <= MENU_WIDTH / 2 - 70 &&
                        y >= MENU_HEIGHT - 90 && y <= MENU_HEIGHT - 60){
                    //点击了左切换键
                    head = tail;//头节点退一步
                    while(tail->next != head){
                        tail = tail->next;//尾节点走到头节点前
                    }
                    break;
                }
                else if(x >= MENU_WIDTH / 2 + 70 && x <= MENU_WIDTH / 2 + 100 &&
                        y >= MENU_HEIGHT - 90 && y <= MENU_HEIGHT - 60){
                    //点击了右切换键
                    head = head->next;
                    tail = tail->next;
                    break;
                }      
            }
        }
    }
    //释放链表内存
    CHAR_NODE *p1 = head;
    while(p1 != tail){
        CHAR_NODE *temp = p1;
        p1 = p1 -> next;
        free(temp);
    }
    free(tail);
}

void showMenu(){
    //初始化界面
    initgraph(MENU_WIDTH, MENU_HEIGHT);
    setbkcolor(BROWN);
    setbkmode(TRANSPARENT);
    BeginBatchDraw();
    //选择玩家人数
    numMenu();
    //选择玩家是否为AI和角色
    for(int i = 0;i < player_num;i++){
        //选择玩家是否为AI
        AIMenu(i);
        //选择玩家角色
        if(!AI[i]){//如果是人类玩家才选择角色
            characterMenu(i);
        }
    }
    //AI玩家随机选择角色
    for(int i = 0;i < player_num;i++){
        if(AI[i]){
            for(int j = 0;j < 8;j++){
                if(!characters[j].chosen){
                    character_choose[i] = j;//记录AI玩家选择的角色编号
                    characters[j].chosen = 1;//标记角色为已选择
                    break;
                }
            }
        }
    }
    //菜单结束
    EndBatchDraw();
    closegraph();
}
