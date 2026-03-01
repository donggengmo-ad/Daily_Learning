#ifndef _AUCTION_H_
#define _AUCTION_H_

/*
对象：
    物品：
        block（货币）
        card（分数卡）
            1分：
                flayer（游戏结束前与某人单独竞拍）
                jade pendent（升级骰子，仅一次）
                business license（拍卖佣金加倍，仅一次）
                Mo Ye（与干将协同，一分卡视为两分卡）
                Feng Shui Coin（与玉龙协同，掷骰子有两次机会）
            2分：
                Gan Jiang（见上）
                Jade Loong（见上）
                The Eagle Tripod 
            3分：
                Jade Burial Suit
                The Bronze Mask
        dice（骰子）
            4面
            6面
            12面
            20面
    人物：
        player（玩家）
            auction（拍卖师）
            bidder（竞拍者）
        character（角色）
    其他：
        round（回合）
过程：
    菜单阶段
        选择玩家人数（3-10人）
    游戏进行
        竞选拍卖师
        选定（或抽取）拍卖品（非2、3分卡隐藏）
        租骰子并同步提交（上轮拍卖师加收1$）（可退出）
        掷骰子比大小       
        每人获得1$
    游戏结束
        使用flayer
*/

#include <graphics.h>

#define PI 3.14159265358979323846
#define HEIGHT 800
#define WIDTH 1000
#define AVATAR_SIZE 70
#define CARD_WIDTH 100
#define CARD_HEIGHT 150
#define DICE_SIZE 200

typedef struct _DICE{
    int size;
    int *score_of_size;
    int price;
    char name[80];
    IMAGE image;
    IMAGE image_small;
}DICE;
typedef struct _CARD{
    int no;//卡牌编号
    IMAGE cover;//卡牌封面
    IMAGE cover_source;//卡牌封面原尺寸图
    char name[80];//卡牌名称
    /*
    1.Flayer
    2.Jade Pendent
    3.Business License
    4.Mo Ye
    5.Feng Shui Coin
    6.Gan Jiang
    7.Jade Loong
    8.The Eagle Tripod
    9.Jade Burial Suit
    10.The Bronze Mask 
    */
    int score;
    int effect;//是否有技能，1有，0无，-1一半，-2失效
    char effect_name[80];//技能名称
    /*
    1.Duel √ duel()中
    2.Priceless √ diceroll内的priceless()中
    3.Acumen √ settleround()内的acumen()中
    4/6.1+1>2 √ settleRound()和printSettleRound()中
    5/7.Fortune √ diceroll()内的fortune()中
    */
   int belong;//卡片归属玩家编号（0-based）
}CARD;
typedef struct _CARD_NODE{//链表栈，存储玩家卡牌
    CARD *card;
    struct _CARD_NODE *next;
    struct _CARD_NODE *prev;
}CARD_NODE;
typedef struct _PLAYER{//以数组形式存在
    char name[80];//玩家名称
    int score;//分数
    int block;//玩家金钱
    int identity;//玩家身份,1拍卖长,0竞拍者
    int ex_auc;//上一轮是否是拍卖长，1是0否
    DICE *dice;//本轮骰子指针
    int dice_roll;//本轮掷骰结果,-1表示平局后不参加下一轮
    int salary;//本轮出价
    IMAGE *avatar;//头像指针
    int AI;//是否为AI玩家，1是0否
    CARD_NODE *card_stack;//玩家卡牌栈顶指针
}PLAYER;
typedef struct _ROUND{//回合信息
    int no;//回合编号
    int state;//回合状态，0菜单，1选举，2选卡，3选骰子，4掷骰子，5结算
    CARD *card;//拍卖物品指针
    int salary;//最低薪资(%)
    int auc_no;//当前最低价玩家编号
    int winner_no;//本轮赢家编号
    int expense;//拍卖费用
    int card_remain;//剩余卡牌数量
}ROUND;
typedef struct _CHARACTER{//游戏角色信息
    char name[80];
    char description[800];
    IMAGE avatar;
    int chosen;//是否被选为玩家角色，1是0否
}CHARACTER;

//menu.cpp
extern int player_num;
extern int AI[];
extern int character_choose[];
void showMenu();
void numMenu();
void AIMenu(int);
void characterMenu(int);

//interface.cpp
extern IMAGE background;
extern IMAGE table;
extern IMAGE closet;
extern IMAGE showcase1;
extern IMAGE showcase2;
void transparentimage(IMAGE* , int, int);
void PutImageZoom(int,int,int,int,IMAGE*);
TCHAR* CharToTCHAR(const char*);
void printBackground(int, int, int, char*);
void printButton(int, int, int, char*, COLORREF);
void printButton(int, char*);
int checkButton(int, int, int, char*, ExMessage);
int checkButton(int, ExMessage, char*);
void printPlayer(int, int, int, int);
int printElection(int);
int printCardSelect();
int printDiceSelect(int);
int printDiceRoll(int);
void printSettleRound(int);
void printSettleGame(int*);
int printDuel(int);
int printDuelCard(int,int);
int printFortune(int);
int printPriceless(int);
int printAcumen(int);

//gaming.cpp
extern CHARACTER characters[];
extern CARD cards[];
extern PLAYER *players;
extern ROUND rounds;
extern DICE dices[];
void shufflePlayers(PLAYER [],int);
void initDice(DICE *,int);
void initCard(CARD *,int);
void initPlayer(PLAYER *,int);
void initGame();
void startGame();
void auctionElect();
void cardSelect();
void diceSelect();
void diceRoll();
int checkTie(int,int);
void settleRound();
void settleGame();
void endGame();
void duel();
int fortune(int,int);
void priceless(int);
int acumen(int);

//AI.cpp
double cardValue(int, CARD);
double aucValue(int,int);
double diceValue(int,DICE);
int electionAI(int);
int cardSelectAI();
int diceSelectAI(int);
int diceRollAI(int);
int duelAI(int);
int duelCardAI(int,int);
int fortuneAI(int);

//socket.cpp
extern int socket_enabled;
int initSocket(const char* server_ip, int server_port);
int sendData(const char* data);
char* receiveData();
void closeSocket();
void sendGameState();
void receiveGameState();

#endif 