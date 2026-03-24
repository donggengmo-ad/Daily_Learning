#include <stdio.h>
#include "auction.h"

void printTest(){
    if(rounds.state == 0){
        //菜单阶段
        printf("game start\n");
        printf("player num: %d\n",player_num);//test
    }
    else if(rounds.state == 1){
        //选举阶段
        printf("\nround %d\n",rounds.no);//test
        printf("\nstate: %d\n",rounds.state);//test
        if(rounds.auc_no == -1){
            printf("no auctioneer\n");//test
        }
        else{
            printf("auctioneer: %s\n",players[rounds.auc_no].name);//test
            printf("salary: %d%\n",rounds.salary);//test
        }
    }
    else if(rounds.state == 2){
        //选卡阶段
        printf("\nstate: %d\n",rounds.state);//test
        printf("card selected: %s\n",rounds.card -> name);//test
    }
    else if(rounds.state == 3){
        //选骰子阶段
        printf("\nstate: %d\n",rounds.state);//test
        for(int i = 0;i < player_num;i++){
            if(i != rounds.auc_no){//非拍卖长玩家
                if(players[i].dice == NULL){//放弃选择
                    printf("%s quit\n",players[i].name);//test
                }
                else{
                    printf("%s selected %d-sided dice\n",players[i].name,players[i].dice -> size);//test
                }
            }
        }
        printf("total expense: %d\n",rounds.expense);//test
    }
    else if(rounds.state == 4){
        //掷骰子阶段
        printf("\nstate: %d\n",rounds.state);//test
        printf("card %s won by %s\n",rounds.card -> name,players[rounds.winner_no].name);//test
    }
    else if(rounds.state == 5){
        //结算阶段
        printf("\nstate: %d\n",rounds.state);//test
        double auctioneer_salary = rounds.expense * rounds.salary / 100.0;
        //向上取整
        if(auctioneer_salary - (int)auctioneer_salary > 0){
            auctioneer_salary = (int)auctioneer_salary + 1;
        }
        else{
            auctioneer_salary = (int)auctioneer_salary;
        }
        printf("auctioneer salary: %d\n",(int)auctioneer_salary);//test
    }
}

int main(){
    //游戏初始化阶段
    initGame();

    //菜单阶段
    showMenu();
    printTest();

    //游戏进行阶段
    startGame();
    while(rounds.card_remain > 0){//无剩余卡牌结束游戏
        //选举阶段
        auctionElect();
        printTest();

        //选卡阶段
        cardSelect();
        printTest();
        
        //选骰子阶段
        diceSelect();
        printTest();

        //掷骰子阶段
        diceRoll();
        printTest();

        //结算阶段
        settleRound();
        printTest();
    }
    //决斗技能结算
    duel();

    //游戏结算阶段
    settleGame();
    
    //退出游戏
    endGame();
    return 0;
}

// int main(){//测试用
//     player_num = 5;
//     initGame();
//     for(int i = 0;i < player_num;i++){
//         AI[i] = 1;
//         character_choose[i] = i;
//     }
//     startGame();
//     auctionElect();
//     cardSelect();
//     diceSelect();
//     diceRoll();
//     settleRound();
//     settleGame();
//     endGame();
//     return 0;
// }
