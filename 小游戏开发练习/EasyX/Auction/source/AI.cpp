//AI决策算法代码文件

#include "auction.h"
#include <math.h>

#define RATE 0.27//金钱与分数兑换比率1$≈0.27分

//AI决策函数都在interface.cpp使用，在每个交互函数读取输入的前一步，决策函数里自带Sleep函数来模拟思考时间

typedef struct _FACTOR{//游戏阶段系数
    //系数数据
    double money;//金钱价值系数
    double score;//分数紧迫度系数
    double combo;//组合可能性系数
    //操作函数
    _FACTOR update(){//根据当前回合信息更新系数
        _FACTOR f = {0};
        //根据回合信息更新系数
        f.money = RATE * (1.0 + rounds.no / 10.0);//轮数越多，金钱价值越高
        f.score = 1.0 + rounds.no / 10.0;//轮数越多，紧迫度越高
        f.combo = 1.0 - (1.0 - rounds.card_remain / 10.0);//剩余卡牌越少，组合可能性越低
        return f;
    }
}FACTOR;

/*

等效分数（Equivalent Score, ES）——即把所有对象折算成"等价于多少最终得分"

赢家平均得分 6.2 分
总资源约 22$（初始12 + 10轮×$1收入）——这是全局资金预算上限
    ->
一个玩家用大约 22$去争夺约6分的得分，因此1$ ≈ 0.27 分 是粗略的基准兑换比

金钱价值：轮数、资金排名、剩余资金、剩余卡牌

卡牌价值：分数、技能、组合与阻断

骰子（金钱->卡牌 转换工具）价值：P(胜出) × 卡牌ES - 租金ES

拍卖师价值：（佣金 - 1$）ES + 选卡控制权的ES - 当轮卡牌机会成本ES

*/

FACTOR K;//游戏阶段系数

//工具函数


//通用策略算法函数
double cardValue(int player_no, CARD c){//评估卡牌价值
    double value = 0.0;
    //基础分数价值
    value += c.score * K.score;
    //技能价值
    if(c.effect == 1){//有技能
        //c.no是1-based
        if(c.no == 1){//Flayer
            value += 2.0;//决斗有翻盘能力,但有概率限制，价值较高
        }
        else if(c.no == 2){//Jade Pendent
            value += 0.5;//一次性升级骰子，价值一般
        }
        else if(c.no == 3){//Business License
            value += 1.0;//一次性拍卖佣金加倍，价值还行
        }
    }
    //一半技能（联动技能）
    else if(c.effect == -1){
        if(c.no == 4){//Mo Ye
            value += 2.0;//翻倍1分卡，价值不错
            if(cards[5].belong != -1){
                value += 2.0 * K.combo;//另一张被收集，组合阻断价值高
            }
        }
        else if(c.no == 6){//Gan Jiang
            value += 2.0;//翻倍1分卡，价值不错
            if(cards[3].belong != -1){
                value += 2.0 * K.combo;//另一张被收集，组合阻断价值高
            }
        }
        else if(c.no == 5){//Feng Shui Coin
            value += 1.5;//二次掷骰，价值高
            if(cards[6].belong != -1){
                value += 1.5 * K.combo;//另一张被收集，组合阻断价值高
            }
        }
        else if(c.no == 7){//Jade Looong
            value += 1.5;//二次掷骰，价值高
            if(cards[4].belong != -1){
                value += 1.5 * K.combo;//另一张被收集，组合阻断价值高
            }
        }
    }
    return value;
}
double aucValue(int player_no,int rate){//为某一薪资率评估价值
    double value = 0.0;
    //估计佣金
    int expense = 0;
    //评估敌人
    for(int i = 0; i < player_num - 1; i++){
        if(i != player_no){
            //记录敌人信息
            int money = players[i].block;
            int score = players[i].score;
            //计算卡牌数量
            CARD_NODE *q = players[i].card_stack;
            int card_count = 0;
            while(q != NULL){
                card_count++;
                q = q -> next;
            }
            //估计拍卖费用
            double fee = 0.0;
            //资金为基础，分数越少越想买，卡牌越多越不想买
            fee = money * (1.7 - score / 10.0) * (1.5 - card_count / 10.0);
            //    (0~22)  (1.7 ~ 0)              (1.5 ~ 0.5)
            if(fee > 20){//资金充足且需求高，买20面骰
                expense += 6;
            }
            else if(fee > 10){//买12面骰
                expense += 4;
            }
            else if(fee > 5){//买6面骰
                expense += 2;
            }
            else if(fee > 1){//基本上会至少买4面骰
                expense += 1;
            }
            else{
                expense += 0;
            }
        }
    }
    //计算预估佣金
    double salary = expense * rate / 100.0;
    if(salary - (int)salary > 0){
        salary = (int)salary + 1;//向上取整
    }
    //成功可能性
    double pos = 1.0 * (1.0 - rate / 100.0);//出价越高，成功可能性越低
    for(int i = 0;i < player_num - 1;i++){
        if(i != player_no){
            if(players[i].salary >= salary){
                pos -= 0.1;//每个竞争对手都降低成功可能性，不完全准确
            }
        }
    } 
    //计算佣金价值
    value = (K.combo * K.money * salary) * pos;//combo系数表示选卡控制权价值，越容易组合控制权价值越高
    return value;
}
double diceValue(int player_no, DICE d){//评估骰子价值
    double value = 0.0;
    //计算卡牌等效分数
    double card_es = cardValue(player_no, *rounds.card);
    //猜测敌人选骰子
    int enemy_dice = 0;//敌人可能选的最高骰子面数
    for(int i = 0;i < player_num - 1;i++){
        //猜测敌人选骰子
        if(i != player_no){
            //记录敌人信息
            int temp;
            int money = players[i].block;
            int score = players[i].score;
            //计算卡牌数量
            CARD_NODE *q = players[i].card_stack;
            int card_count = 0;
            while(q != NULL){
                card_count++;
                q = q -> next;
            }
            //估计拍卖费用
            double fee = 0.0;
            //资金为基础，分数越少越想买，卡牌越多越不想买，卡牌价值越高越想买
            fee = money * (1.7 - score / 10.0) * (1.5 - card_count / 10.0) * card_es;
            //    (0~22)  (1.7 ~ 0)              (1.5 ~ 0.5)
            if(fee > 20){//资金充足且需求高，买20面骰
                temp = 20;
            }
            else if(fee > 10){//买12面骰
                temp = 12;
            }
            else if(fee > 5){//买6面骰
                temp = 6;
            }
            else if(fee > 1){//基本上会至少买4面骰
                temp = 4;
            }
            else{
                temp = 0;
            }
            if(temp > enemy_dice){
                enemy_dice = temp;
            }
        }
    }
    //计算胜出概率
    double pos = 0.0;
    int win = 0;//胜出情况数
    int total = 0;//总情况数
    for(int i = 1;i <= d.size;i++){
        for(int j = 1;j <= enemy_dice;j++){
            total++;
            if(i > j){
                win++;
            }
        }
    }
    if(total > 0){
        pos = 1.0 * win / total;
    }
    //计算租金等效分数
    double rent_es = d.price * K.money;//租金价值与金钱价值系数相关
    //计算骰子价值
    value = card_es * pos - rent_es;
    return value;
}

//具体环节决策函数
int electionAI(int player_no){
    //更新系数
    K = K.update();
    if(players[player_no].AI == 0){
        //非AI玩家不决策
        return 0;
    }
    //模拟思考
    Sleep(1000);
    //获取各决策分数
    int best_rate = 100;
    int max_value = -1;
    for(int rate = 100;rate >= 10;rate -= 10){
        double value = aucValue(player_no, rate);
        if(value > max_value){
            max_value = value;
            best_rate = rate;
        }
    }
    return best_rate;
}
int cardSelectAI(){
    if(players[rounds.auc_no].AI == 0){
        //非AI玩家不决策
        return -1;
    }
    //模拟思考
    Sleep(2000);
    //获取各卡牌价值,选择价值最低的卡牌（因为自己不能买）
    int best_card = -1;
    double min_value = 100.0;
    for(int i = 0;i < 10;i++){
        if(cards[i].belong == -1){//未被购买的卡牌
            double value = cardValue(rounds.auc_no, cards[i]);
            if(value < min_value){
                min_value = value;
                best_card = i;
            }
        }
    }
    return best_card;
}
int diceSelectAI(int player_no){
    if(players[player_no].AI == 0){
        //非AI玩家不决策
        return 0;
    }
    //模拟思考
    Sleep(1000);
    int best_dice = -1;
    double max_value = -1.0;
    for(int i = 0;i < 4;i++){
        if(players[player_no].block >= dices[i].price){
            //计算骰子价值
            double value = diceValue(player_no, dices[i]);
            if(value > max_value){
                max_value = value;
                best_dice = dices[i].size;
            }
        }
    }
    return best_dice;
}
int diceRollAI(int player_no){
    if(players[player_no].AI == 0){
        //非AI玩家不决策
        return 0;
    }
    //模拟思考
    Sleep(1000);
    if(players[player_no].dice != NULL){
        //有骰子，正常掷骰
        return 1;
    }
    else{
        //没有骰子，掷4面骰
        return 0;
    }
}
int duelAI(int player_no){
    if(players[player_no].AI == 0){
        //非AI玩家不决策
        return -1;
    }
    //模拟思考
    Sleep(1000);
    //找出最佳卡牌持有人
    int best_player = -1;
    int best_value = -1;
    for(int i = 0;i < 10;i++){
        if(cards[i].belong != -1 && cards[i].belong != player_no){//其他玩家的卡牌
            double value = cardValue(player_no, cards[i]);
            if(value > best_value){
                best_value = value;
                best_player = cards[i].belong;
            }
        }
    }
    return best_player;
}
int duelCardAI(int player_no,int target_no){
    if(players[player_no].AI == 0){
        //非AI玩家不决策
        return -1;
    }
    //模拟思考
    Sleep(1000);
    //找出最佳卡牌
    int best_card = -1;
    double best_value = -1.0;
    CARD_NODE *p = players[target_no].card_stack;
    while(p != NULL){
        double value = cardValue(player_no, *(p -> card));
        if(value > best_value){
            best_value = value;
            best_card = p -> card -> no;
        }
        p = p -> next;
    }
    return best_card;
}
int fortuneAI(int player_no){
    if(players[player_no].AI == 0){
        //非AI玩家不决策
        return 0;
    }
    //模拟思考
    Sleep(1000);
    //简单策略：如果有风水轮和玉龙,则使用技能
    if((cards[4].belong == player_no && cards[4].effect == 1) &&
     (cards[6].belong == player_no && cards[6].effect == 1)){
        return 1;//使用技能
    }
    else{
        return 0;//不使用技能
    }
}

