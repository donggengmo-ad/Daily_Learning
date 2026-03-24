//游戏主要逻辑代码文件

#include "auction.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

CHARACTER characters[8];
CARD cards[10];
DICE dices[4];
PLAYER *players;
ROUND rounds;

//工具函数
void shufflePlayers(PLAYER arr[], int size) {// 洗牌函数：打乱数组顺序（Fisher-Yates 算法，高效公平）
    for (int i = size - 1; i > 0; i--) {
        // 生成0~i之间的随机索引
        int j = rand() % (i + 1);
        // 交换arr[i]和arr[j]
        PLAYER temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}
//初始化函数
void initDice(DICE *d,int s){
    //设置骰子面数
    d -> size = s;
    //根据面数确定信息
    if(s == 4){
        d -> price = 1;
        loadimage(&d -> image,_T("images\\dice4.png"),DICE_SIZE,DICE_SIZE);
        loadimage(&d -> image_small,_T("images\\dice4.png"),DICE_SIZE / 3,DICE_SIZE / 3);
        strcpy(d -> name,"4-sided Dice");
    }
    else if(s == 6){
        d -> price = 2;
        loadimage(&d -> image,_T("images\\dice6.png"),DICE_SIZE,DICE_SIZE);
        loadimage(&d -> image_small,_T("images\\dice6.png"),DICE_SIZE / 3,DICE_SIZE / 3);
        strcpy(d -> name,"6-sided Dice");
    }
    else if(s == 12){
        d -> price = 4;
        loadimage(&d -> image,_T("images\\dice12.png"),DICE_SIZE,DICE_SIZE);
        loadimage(&d -> image_small,_T("images\\dice12.png"),DICE_SIZE / 3,DICE_SIZE / 3);
        strcpy(d -> name,"12-sided Dice");
    }
    else if(s == 20){
        d -> price = 6;
        loadimage(&d -> image,_T("images\\dice20.png"),DICE_SIZE,DICE_SIZE);
        loadimage(&d -> image_small,_T("images\\dice20.png"),DICE_SIZE / 3,DICE_SIZE / 3);
        strcpy(d -> name,"20-sided Dice");
    }
    //创建每面分数数组
    d -> score_of_size = (int *)malloc(sizeof(int) * s);
    for(int i = 0;i < s;i++){
        d -> score_of_size[i] = i + 1;//0-based的面
    }
}
void initCard(CARD *c,int n){
    //确定卡牌编号
    c -> no = n;
    //设置卡牌名、分数、技能
    if(n == 1){
        strcpy(c -> name,"Flayer");
        c -> score = 1;
        c -> effect = 1;
        strcpy(c -> effect_name,"Duel");
    }
    else if(n == 2){
        strcpy(c -> name,"Jade Pendent");
        c -> score = 1;
        c -> effect = 1;
        strcpy(c -> effect_name,"Priceless");
    }
    else if(n == 3){
        strcpy(c -> name,"Business License");
        c -> score = 1;
        c -> effect = 1;
        strcpy(c -> effect_name,"Acumen");
    }
    else if(n == 4){
        strcpy(c -> name,"Mo Ye");
        c -> score = 1;
        c -> effect = -1;
        strcpy(c -> effect_name,"1+1>2");
    }
    else if(n == 6){
        strcpy(c -> name,"Gan Jiang");
        c -> score = 2;
        c -> effect = -1;
        strcpy(c -> effect_name,"1+1>2");
    }
    else if(n == 5){
        strcpy(c -> name,"Feng Shui Coin");
        c -> score = 1;
        c -> effect = -1;
        strcpy(c -> effect_name,"Fortune");
    }
    else if(n == 7){
        strcpy(c -> name,"Jade Loong");
        c -> score = 2;
        c -> effect = -1;
        strcpy(c -> effect_name,"Fortune");
    }
    else if(n == 8){
        strcpy(c -> name,"The Eagle Tripod");
        c -> score = 3;
        c -> effect = 0;
        strcpy(c -> effect_name,"None");
    }
    else if(n == 9){
        strcpy(c -> name,"Jade Burial Suit");
        c -> score = 3;
        c -> effect = 0;
        strcpy(c -> effect_name,"None");
    }
    else if(n == 10){
        strcpy(c -> name,"The Bronze Mask");
        c -> score = 3;
        c -> effect = 0;
        strcpy(c -> effect_name,"None");
    }
    //初始化归属
    c -> belong = -1;
}
void initPlayer(PLAYER *p,int i){
    int cha = character_choose[i];
    int ai = AI[i];
    strcpy(p -> name,characters[cha].name);
    p -> score = 0;
    p -> block = 12;
    p -> identity = 0;
    p -> ex_auc = 0;
    p -> avatar = &characters[cha].avatar;
    p -> salary = 0;
    p -> dice = NULL;
    p -> dice_roll = 0;
    p -> AI = ai;
    p -> card_stack = NULL;
}
void initGame(){
    srand((unsigned int) time(NULL));
    //载入角色头像
    loadimage(&characters[0].avatar,_T("images\\avatar1.png"),AVATAR_SIZE,AVATAR_SIZE);
    loadimage(&characters[1].avatar,_T("images\\avatar2.png"),AVATAR_SIZE,AVATAR_SIZE);
    loadimage(&characters[2].avatar,_T("images\\avatar3.png"),AVATAR_SIZE,AVATAR_SIZE);
    loadimage(&characters[3].avatar,_T("images\\avatar4.png"),AVATAR_SIZE,AVATAR_SIZE);
    loadimage(&characters[4].avatar,_T("images\\avatar5.png"),AVATAR_SIZE,AVATAR_SIZE);
    loadimage(&characters[5].avatar,_T("images\\avatar6.png"),AVATAR_SIZE,AVATAR_SIZE);
    loadimage(&characters[6].avatar,_T("images\\avatar7.png"),AVATAR_SIZE,AVATAR_SIZE);
    loadimage(&characters[7].avatar,_T("images\\avatar8.png"),AVATAR_SIZE,AVATAR_SIZE);
    //载入角色姓名
    strcpy(characters[0].name,"Ethan");
    strcpy(characters[1].name,"Layla");
    strcpy(characters[2].name,"Marcus");
    strcpy(characters[3].name,"Leo");
    strcpy(characters[4].name,"Chloe");
    strcpy(characters[5].name,"Arthur");
    strcpy(characters[6].name,"Owen");
    strcpy(characters[7].name,"Noah");
    //载入角色描述
    strcpy(characters[0].description,"A calm researcher in the medical field,\nhe excels at rational situation analysis \nand can always accurately evaluate the practical value of items during auctions.");
    strcpy(characters[1].description,"A workplace-style strategic player with clear logic,\nshe prefers to gain the initiative in auctions through resource integration.");
    strcpy(characters[2].description,"A veteran player who has experienced complex games, \nhe is good at predicting opponents' moves with experience, \nand his auction style is steady and deterrent.");
    strcpy(characters[3].description,"An energetic new player who favors adventurous auctions;\nhe occasionally secures unexpected high-quality items by relying on intuition.");
    strcpy(characters[4].description,"A player with an inherent collaboration trait\nshe not only calculates profits rationally but also balances \non-field resources through cooperative transactions.");
    strcpy(characters[5].description,"A wise man specializing in long-term planning; \nhe doesn't rush to compete for immediate items \nand is skilled at waiting for the right moment to bid on high-value props.");   
    strcpy(characters[6].description,"A workplace-newcomer-style player who is good at leveraging rule details, \noften steadily accumulating assets with low-risk strategies.");
    strcpy(characters[7].description,"A goal-oriented powerhouse who focuses on bidding for core items, \nand excels at concentrating resources to secure key props.");
    //初始化角色选择状态
    for(int i = 0;i < 8;i++){
        characters[i].chosen = 0;
    }

    //载入背景图片
    loadimage(&background,_T("images\\background.png"),WIDTH,HEIGHT);
    loadimage(&table,_T("images\\table.png"),HEIGHT - 200,HEIGHT - 200);
    loadimage(&closet,_T("images\\closet.png"),WIDTH * 3 / 4,HEIGHT);
    loadimage(&showcase1,_T("images\\showcase1.png"),DICE_SIZE + 50,DICE_SIZE + 50);
    loadimage(&showcase2,_T("images\\showcase2.png"),DICE_SIZE + 40,DICE_SIZE + 40);

    //初始化卡牌
    for(int i = 0;i < 10;i++){
        initCard(&cards[i],i + 1);
    }
    //载入卡牌图片
    loadimage(&cards[0].cover,_T("images\\Flayer.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[0].cover_source,_T("images\\Flayer.png"));
    loadimage(&cards[1].cover,_T("images\\JadePendant.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[1].cover_source,_T("images\\JadePendant.png"));
    loadimage(&cards[2].cover,_T("images\\License.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[2].cover_source,_T("images\\License.png"));
    loadimage(&cards[3].cover,_T("images\\MoYe.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[3].cover_source,_T("images\\MoYe.png"));
    loadimage(&cards[4].cover,_T("images\\FengShui.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[4].cover_source,_T("images\\FengShui.png"));
    loadimage(&cards[5].cover,_T("images\\GanJiang.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[5].cover_source,_T("images\\GanJiang.png"));
    loadimage(&cards[6].cover,_T("images\\JadeLoong.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[6].cover_source,_T("images\\JadeLoong.png"));
    loadimage(&cards[7].cover,_T("images\\Eagle.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[7].cover_source,_T("images\\Eagle.png"));
    loadimage(&cards[8].cover,_T("images\\Suit.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[8].cover_source,_T("images\\Suit.png"));
    loadimage(&cards[9].cover,_T("images\\Mask.png"),CARD_WIDTH,CARD_HEIGHT);
    loadimage(&cards[9].cover_source,_T("images\\Mask.png"));

    //初始化骰子
    initDice(&dices[0],4);
    initDice(&dices[1],6);
    initDice(&dices[2],12);
    initDice(&dices[3],20);

    //初始化菜单信息
    for(int i = 0;i < 8;i++){
        AI[i] = 0;
        character_choose[i] = 0;
    }
}
//游戏环节函数
void startGame(){
    //初始化玩家
    players = (PLAYER *)malloc(sizeof(PLAYER) * player_num);
    for(int i = 0;i < player_num;i++){
        initPlayer(&players[i], i);
    }
    shufflePlayers(players, player_num);//打乱玩家顺序
    //初始化回合信息
    rounds.salary = 101;
    rounds.auc_no = -1;
    rounds.winner_no = -1;
    rounds.no = 1;
    rounds.state = 0;
    rounds.card = NULL;
    rounds.expense = 0;
    rounds.card_remain = 10;
    //初始化图形页面
    initgraph(WIDTH, HEIGHT);
    setbkcolor(BLACK);
    setbkmode(TRANSPARENT);
    BeginBatchDraw();
    cleardevice();
    putimage(0, 0, &background);
    FlushBatchDraw();
}
void auctionElect(){
    //数据处理
    rounds.state = 1;//设置回合状态为选举
    rounds.salary = 101;//初始最低价设为不可能值
    rounds.auc_no = -1;//初始最低价玩家编号设为无效值
    int first_player = rounds.no % player_num;//本轮第一个出价玩家
    //输出画面
    for(int i = first_player; ;i++){
        players[i].salary = printElection(i);
        //检查放弃
        if(players[i].salary != -1){
            //更新薪资
            if(players[i].salary < rounds.salary){
                rounds.salary = players[i].salary;
                rounds.auc_no = i;
            }
        }
        if(rounds.salary == 10){
            //达到最低价，提前结束选举
            break;
        }
        if(i == player_num - 1){
            //最后一个玩家出价完毕，继续从第一个玩家开始
            i = -1;
        }
        if(i == first_player - 1){
            //所有玩家均已出价完毕，结束选举
            break;
        }
    }
    if(rounds.auc_no != -1){
        //设置拍卖长身份
        players[rounds.auc_no].identity = 1;
        //其他玩家身份设为竞拍者
        for(int i = 0;i < player_num;i++){
            if(i != rounds.auc_no){
                players[i].identity = 0;
            }
        }
    }
    else{
        //无人当选拍卖长
        for(int i = 0;i < player_num;i++){
            players[i].identity = 0;
        }
        //随机拍卖
        rounds.auc_no = -1;
    }
}
void cardSelect(){
    rounds.state = 2;
    if(rounds.auc_no != -1){//有拍卖长
        //拍卖长选择卡牌
        int selected_no = printCardSelect();
        rounds.card = &cards[selected_no];
    }
    else{//无人当选拍卖长
        //随机选择卡牌
        int rand_no = rand() % 10;
        rounds.card = &cards[rand_no];
    }
}
void diceSelect(){
    rounds.state = 3;
    for(int i = 0;i < player_num;i++){
        //除拍卖长外，其他玩家选择骰子
        if(i != rounds.auc_no){
            int selected_size = 0;
            //玩家选择骰子面数
            selected_size = printDiceSelect(i);
            //检查放弃
            if(selected_size == -1){
                players[i].dice = NULL;
            }
            else{
                //分配骰子指针
                for(int j = 0;j < 4;j++){
                    if(dices[j].size == selected_size){
                        players[i].dice = &dices[j];
                        break;
                    }
                }
                //更新玩家金钱
                players[i].block -= players[i].dice -> price;
                rounds.expense += players[i].dice -> price;
                if(players[i].ex_auc == 1){
                    //上一轮为拍卖长，加价1
                    players[i].block -= 1;
                    rounds.expense += 1;
                }
            }
        }
    }
}
void diceRoll(){
    rounds.state = 4;
    int max_score = -1;
    int winner_no = -1;
    //玩家掷骰子
    for(int i = 0;i < player_num;i++){
        if(i != rounds.auc_no && players[i].dice != NULL){//拍卖长不掷骰子，放弃的玩家不掷骰子
            priceless(i);//处理Priceless技能
            int roll_result = printDiceRoll(i);
            players[i].dice_roll = roll_result;//记录掷骰结果
            roll_result = fortune(i, roll_result); //处理Fortune技能
            if(roll_result == -1){//小于等于1人参加，直接结束掷骰环节
                //寻找唯一玩家
                int only_player = -1;
                for(int j = 0;j < player_num;j++){
                    if(j != rounds.auc_no && players[j].dice != NULL){
                        only_player = j;
                        break;
                    }
                }
                if(only_player == -1){
                    //无人参加掷骰，直接返回
                    return;
                }
                else{
                    //唯一玩家获胜
                    winner_no = only_player;
                    //更新卡牌归属
                    rounds.card -> belong = winner_no;
                    rounds.winner_no = winner_no;
                    //更新玩家卡牌栈
                    if(players[winner_no].card_stack == NULL){
                        //卡牌栈为空，创建新节点
                        CARD_NODE *new_p = (CARD_NODE *)malloc(sizeof(CARD_NODE));
                        new_p -> card = rounds.card;
                        new_p -> next = NULL;
                        new_p -> prev = NULL;
                        players[winner_no].card_stack = new_p;
                    }
                    else{
                        //卡牌栈不为空，在栈顶添加新节点
                        CARD_NODE *new_p = (CARD_NODE *)malloc(sizeof(CARD_NODE));
                        new_p -> card = rounds.card;
                        new_p -> next = players[winner_no].card_stack;
                        new_p -> prev = NULL;
                        players[winner_no].card_stack -> prev = new_p;
                        players[winner_no].card_stack = new_p;
                    }
                    return;
                }
            }
            else{
                players[i].dice_roll = roll_result;//记录掷骰结果
                //更新最高分数
                if(roll_result > max_score){
                    max_score = roll_result;
                    winner_no = i;
                }
            }
        }
    }
    if(winner_no != -1){//有玩家参加掷骰
        //检查平局
        int temp_winner_no = checkTie(winner_no,max_score);
        if(temp_winner_no != winner_no){
            //平局处理后赢家发生变化，更新赢家编号
            winner_no = temp_winner_no;
        }
        //更新卡牌归属
        rounds.card -> belong = winner_no;
        rounds.winner_no = winner_no;
        //更新玩家卡牌栈
        if(players[winner_no].card_stack == NULL){
            //卡牌栈为空，创建新节点
            CARD_NODE *new_p = (CARD_NODE *)malloc(sizeof(CARD_NODE));
            new_p -> card = rounds.card;
            new_p -> next = NULL;
            new_p -> prev = NULL;
            players[winner_no].card_stack = new_p;
        }
        else{
            //卡牌栈不为空，在栈顶添加新节点
            CARD_NODE *new_p = (CARD_NODE *)malloc(sizeof(CARD_NODE));
            new_p -> card = rounds.card;
            new_p -> next = players[winner_no].card_stack;
            new_p -> prev = NULL;
            players[winner_no].card_stack -> prev = new_p;
            players[winner_no].card_stack = new_p;
        }
    }
}
int checkTie(int winner_no,int max_score){//检查是否平局,平局返回新赢家编号，否则返回原赢家编号
    //检查平局
    int tie_players[10] = {0};//记录平局玩家编号
    for(int i = 0;i < player_num - 1;i++){
        if(i == winner_no){//找到第一个获胜者
            for(int j = i + 1;j < player_num;j++){
                if(players[i].dice_roll == players[j].dice_roll){
                    //平局
                    tie_players[i] = 1;
                    tie_players[j] = 1;
                }
            }
            break;
        }
    }
    //平局处理
    if(tie_players[winner_no] == 1){
        //更新点数
        for(int i = 0;i < player_num;i++){
            if(tie_players[i] == 1){
                //平局玩家点数重置为0，准备重新掷骰
                players[i].dice_roll = 0;
            }
            else{
                //非平局玩家不参与重新掷骰
                players[i].dice_roll = -1;
            }
        }
        //重新掷骰子
        for(int i = 0;i < player_num;i++){
            if(tie_players[i] == 1){
                //重新掷骰子
                int roll_result = printDiceRoll(i);
                players[i].dice_roll = roll_result;//记录掷骰结果
                roll_result = fortune(i, roll_result); //处理Fortune技能
                //更新最高分数
                if(roll_result > max_score){
                    max_score = roll_result;
                    winner_no = i;
                }
            }
        }
        //递归检查平局
        int temp_winner_no = checkTie(winner_no,max_score);
        if(temp_winner_no != winner_no){
            //平局处理后赢家发生变化，更新赢家编号
            winner_no = temp_winner_no;
        }
    }
    return winner_no;
}
void settleRound(){
    //玩家结算
    for(int i = 0;i < player_num;i++){
        //重置玩家身份
        players[i].identity = 0;
        //记录上一轮拍卖长身份
        if(i == rounds.auc_no){
            players[i].ex_auc = 1;
        }
        else{
            players[i].ex_auc = 0;
        }
        //金钱发放
        players[i].block += 1;
        //重置玩家薪资
        players[i].salary = 0;
        //重置玩家骰子
        players[i].dice = NULL;
        players[i].dice_roll = 0;
        //技能合成
        if(cards[3].belong == i && cards[5].belong == i){
            //拥有干将和莫邪，合成1+1>2技能
            cards[3].effect = 1;
            cards[5].effect = 1;
        }
        if(cards[4].belong == i && cards[6].belong == i){
            //拥有风水轮和玉龙，合成Fortune技能
            cards[4].effect = 1;
            cards[6].effect = 1;
        }
        //计算分数
        players[i].score = 0;
        for(int j = 0;j < 10;j++){
            if(cards[j].belong == i){
                players[i].score += cards[j].score;
                if(cards[j].score == 1 && cards[3].belong == i && cards[5].belong == i){
                    //有1+1>2技能的额外加分
                    players[i].score ++;
                }
            }
        }
    }
    //拍卖长薪资发放
    double auctioneer_salary = 0;
    if(rounds.auc_no != -1){
        auctioneer_salary = rounds.expense * rounds.salary / 100.0;
        //向上取整
        if(auctioneer_salary - (int)auctioneer_salary > 0){
            auctioneer_salary = (int)auctioneer_salary + 1;
        }
        else{
            auctioneer_salary = (int)auctioneer_salary;
        }
        //更新拍卖长金钱
        players[rounds.auc_no].block += auctioneer_salary;
    }
    //处理Acumen技能
    auctioneer_salary = acumen((int)auctioneer_salary);
    //输出结算画面
    printSettleRound((int)auctioneer_salary);
    //回合结算
    rounds.state = 5;
    rounds.card = NULL;
    rounds.salary = 101;
    rounds.auc_no = -1;
    rounds.winner_no = -1;
    rounds.no++;
    rounds.expense = 0;
    rounds.card_remain = 0;
    //计算剩余卡牌数量
    for(int i = 0;i < 10;i++){
        if(cards[i].belong == -1){
            rounds.card_remain++;
        }
    }
}
void settleGame(){
    //计算最高分玩家
    int max_score = 0;
    int winner[8] = {0};//记录最高分玩家编号
    int winner_count = 0;//记录最高分玩家数量
    //寻找最高分数
    for(int i = 0;i < player_num;i++){
        if(players[i].score > max_score){
            max_score = players[i].score;
        }
    }
    //寻找最高分玩家
    for(int i = 0;i < player_num;i++){
        if(players[i].score == max_score){
            winner[i] = 1;
            winner_count++;
        }
    }
    //高分玩家比金钱
    if(winner_count > 1){
        int max_block = 0;
        //寻找最高金钱
        for(int i = 0;i < player_num;i++){
            if(winner[i] == 1 && players[i].block > max_block){
                max_block = players[i].block;
            }
        }
        //寻找最高金钱玩家
        for(int i = 0;i < player_num;i++){
            if(winner[i] == 1 && players[i].block == max_block){
                winner[i] = 1;//记录最终赢家
            }
            else if(winner[i] == 1){
                winner[i] = 0;//非最高金钱玩家不再是赢家
            }
        }
    }
    //输出最终结果
    printSettleGame(winner);
}
void endGame(){
    //释放动态分配内存
    for(int i = 0;i < 4;i++){
        free(dices[i].score_of_size);
    }
    free(players);
    //释放玩家卡牌栈
    for(int i = 0;i < player_num;i++){
        CARD_NODE *top = players[i].card_stack;
        while(top != NULL){
            CARD_NODE *temp = top -> next;
            free(top);
            top = temp;
        }
    }
    //关闭图形页面
    EndBatchDraw();
    closegraph();
}
//技能函数
void duel(){
    //决斗阶段
    if(cards[0].belong != -1 && cards[0].effect == 1){
        //持有Flayer的玩家触发决斗技能
        int player_no = cards[0].belong;
        int target_no = printDuel(player_no);
        if(target_no != -1){//检查放弃
            //选择卡牌
            int card_no = printDuelCard(player_no,target_no);
            rounds.card = &cards[card_no];
            //设置点数
            for(int i = 0;i < player_num;i++){
                if(i == player_no || i == target_no){
                    //参与决斗的玩家准备掷骰
                    players[i].dice_roll = 0;
                }
                else{
                    //非参与决斗的玩家不参与掷骰
                    players[i].dice_roll = -1;
                }
            }
            //记录当前卡牌归属
            int belong_no = cards[card_no].belong;
            //选骰子
            for(int i = 0;i < player_num;i++){
                if(i == player_no || i == target_no){
                    int selected_size = 0;
                    //玩家选择骰子面数
                    selected_size = printDiceSelect(i);
                    //检查放弃
                    if(selected_size == -1){
                        players[i].dice = NULL;
                    }
                    else{
                        //分配骰子指针
                        for(int j = 0;j < 4;j++){
                            if(dices[j].size == selected_size){
                                players[i].dice = &dices[j];
                                break;
                            }
                        }
                        //更新玩家金钱
                        players[i].block -= players[i].dice -> price;
                        rounds.expense += players[i].dice -> price;
                        if(players[i].ex_auc == 1){
                            //上一轮为拍卖长，加价1
                            players[i].block -= 1;
                            rounds.expense += 1;
                        }
                    }
                }
            }
            //进行掷骰
            diceRoll();
            //归属变化，卡牌出栈（忽然发现这张卡不一定是栈顶，不应该用栈的，草了）
            if(rounds.card -> belong != belong_no){
                CARD_NODE *p = players[belong_no].card_stack;
                //还得先找这张牌的节点
                while(p -> card != rounds.card){
                    p = p -> next;
                }
                //出栈
                if(p -> prev == NULL){
                    //栈顶节点
                    players[belong_no].card_stack = p -> next;
                    free(p);
                }
                else{
                    if(p -> next == NULL){
                        //栈底节点
                        p -> prev -> next = NULL;
                        free(p);
                    }
                    else{
                        //中间节点
                        p -> prev -> next = p -> next;
                        p -> next -> prev = p -> prev;
                        free(p);
                    }
                }
            }
            //Flayer技能失效
            cards[0].effect = -2;
            //联动技能暂时失效，后续再关联，处理联动排被拆分状况
            cards[3].effect = -1;
            cards[5].effect = -1;
            cards[4].effect = -1;
            cards[6].effect = -1;
        }
    }
}
int fortune(int player_no,int result_1){
    //处理Fortune技能
    int roll_result;
    if(cards[4].belong == player_no &&  cards[6].belong == player_no){
        //显示Fortune技能界面，玩家选择是否使用技能
        int result_2 = printFortune(player_no);
        //保留两个结果中较高的一个
        if(result_2 > result_1){
            roll_result = result_2;
        }
        else{
            roll_result = result_1;
        }
    }
    else{
        //没有Fortune技能，直接返回原始掷骰结果
        roll_result = result_1;
    }
    return roll_result;
}
void priceless(int player_no){
    //处理Priceless技能
    if(cards[1].belong == player_no && players[player_no].dice -> size != 20){
        //持有Jade Pendant的玩家触发Priceless技能
        int op = printPriceless(player_no);
        if(op){
            //玩家选择使用技能，骰子升级
            if(players[player_no].dice -> size == 4){
                players[player_no].dice = &dices[1];//升级为6面骰
            }
            else if(players[player_no].dice -> size == 6){
                players[player_no].dice = &dices[2];//升级为12面骰
            }
            else if(players[player_no].dice -> size == 12){
                players[player_no].dice = &dices[3];//升级为20面骰
            }
            //技能失效
            cards[1].effect = -2;
        }
    }
}
int acumen(int salary){
    //处理Acumen技能
    if(cards[2].belong == rounds.auc_no && rounds.auc_no != -1 && cards[2].effect == 1){
        //持有Business License的玩家触发Acumen技能
        int op = printAcumen(salary);
        if(op){
            //玩家选择使用技能，薪资翻倍
            players[rounds.auc_no].block += salary;//再次发放原薪资
            salary *= 2;
            //技能失效
            cards[2].effect = -2;
        }
    }
    return salary;
}
