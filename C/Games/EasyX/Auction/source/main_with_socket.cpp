#include <stdio.h>
#include "auction.h"

void printTest(){
    if(rounds.state == 0){
        printf("game start\n");
        printf("player num: %d\n",player_num);
    }
    else if(rounds.state == 1){
        printf("\nround %d\n",rounds.no);
        printf("\nstate: %d\n",rounds.state);
        if(rounds.auc_no == -1){
            printf("no auctioneer\n");
        }
        else{
            printf("auctioneer: %s\n",players[rounds.auc_no].name);
            printf("salary: %d%\n",rounds.salary);
        }
    }
    else if(rounds.state == 2){
        printf("\nstate: %d\n",rounds.state);
        printf("card selected: %s\n",rounds.card -> name);
    }
    else if(rounds.state == 3){
        printf("\nstate: %d\n",rounds.state);
        for(int i = 0;i < player_num;i++){
            if(i != rounds.auc_no){
                if(players[i].dice == NULL){
                    printf("%s quit\n",players[i].name);
                }
                else{
                    printf("%s selected %d-sided dice\n",players[i].name,players[i].dice -> size);
                }
            }
        }
        printf("total expense: %d\n",rounds.expense);
    }
    else if(rounds.state == 4){
        printf("\nstate: %d\n",rounds.state);
        printf("card %s won by %s\n",rounds.card -> name,players[rounds.winner_no].name);
    }
    else if(rounds.state == 5){
        printf("\nstate: %d\n",rounds.state);
        double auctioneer_salary = rounds.expense * rounds.salary / 100.0;
        if(auctioneer_salary - (int)auctioneer_salary > 0){
            auctioneer_salary = (int)auctioneer_salary + 1;
        }
        else{
            auctioneer_salary = (int)auctioneer_salary;
        }
        printf("auctioneer salary: %d\n",(int)auctioneer_salary);
    }
}

int main(){
    const char* SERVER_IP = "127.0.0.1";
    const int SERVER_PORT = 8888;

    if(initSocket(SERVER_IP, SERVER_PORT) == 0){
        printf("Socket连接成功\n");
    } else {
        printf("Socket连接失败，游戏将以单机模式运行\n");
    }

    initGame();
    showMenu();
    printTest();

    if(socket_enabled){
        sendGameState();
        receiveGameState();
    }

    startGame();
    while(rounds.card_remain > 0){
        auctionElect();
        printTest();
        if(socket_enabled){
            sendGameState();
            receiveGameState();
        }

        cardSelect();
        printTest();
        if(socket_enabled){
            sendGameState();
            receiveGameState();
        }
        
        diceSelect();
        printTest();
        if(socket_enabled){
            sendGameState();
            receiveGameState();
        }

        diceRoll();
        printTest();
        if(socket_enabled){
            sendGameState();
            receiveGameState();
        }

        settleRound();
        printTest();
        if(socket_enabled){
            sendGameState();
            receiveGameState();
        }
    }

    duel();
    settleGame();
    endGame();

    if(socket_enabled){
        closeSocket();
    }

    return 0;
}
