#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>

#define GAP 70 // 网格间距
#define SIZE 40 // 棋子大小
#define QX 19683 // Q表的状态数(3^9)
#define QY 9 // Q表的动作数

int board[3][3] = {0}; // 棋盘状态，0表示空格，1表示O，-1表示X
int currentPlayer = 1; // 当前玩家，1表示O，-1表示X
double qTable[QX][QY] = {0}; // Q表
double epsilon = 0.9; // 探索率
double alpha = 0.1; // 学习率
double gamma_f = 0.9; // 折扣因子


//游戏框架
void drawCross(int i,int j){//传入抽象坐标
    int x = GAP * (i + 1) + GAP / 2;
    int y = GAP * (j + 1) + GAP / 2;
    setlinestyle(PS_SOLID, 3);
    line(x - SIZE / 2, y - SIZE / 2, x + SIZE / 2, y + SIZE / 2);
    line(x - SIZE / 2, y + SIZE / 2, x + SIZE / 2, y - SIZE / 2);
}
void drawRing(int i,int j){//传入抽象坐标
    int x = GAP * (i + 1) + GAP / 2;
    int y = GAP * (j + 1) + GAP / 2;
    setlinestyle(PS_SOLID, 3);
    circle(x, y, SIZE / 2);
}
void drawGame(){
    // 绘制网格
    for(int i = 1;i <= 4;i++){
        setlinestyle(PS_SOLID, 1);
        line(GAP * i, GAP ,GAP * i, GAP * 4);//竖线
        line(GAP, GAP * i, GAP * 4, GAP * i);//横线
    }
    // 绘制棋子
    for(int i = 0;i < 3;i++){
        for(int j = 0;j < 3;j++){
            if(board[i][j] == 1){
                drawRing(i,j);
            }else if(board[i][j] == -1){
                drawCross(i,j);
            }
        }
    }
}
void playerMove(){
    //读取输入
    while(true){
        ExMessage msg;
        msg = getmessage(EX_MOUSE);
        if(msg.lbutton){
            for(int i = 0;i < 3;i++){
                for(int j = 0;j < 3;j++){
                    int x = GAP * (i + 1) + GAP / 2;
                    int y = GAP * (j + 1) + GAP / 2;
                    if(msg.x > x - SIZE / 2 && msg.x < x + SIZE / 2
                        && msg.y > y - SIZE / 2 && msg.y < y + SIZE / 2){
                        //点击了第i行第j列
                        if(board[i][j] == 0){
                            //落子
                            board[i][j] = currentPlayer;
                            currentPlayer *= -1; //切换玩家
                            return;
                        }
                    }
                }
            }
        }
    }
}
int checkWin(){
    int sum;
    // 检查每行
    for(int i = 0; i < 3; i++){
        sum = board[i][0] + board[i][1] + board[i][2];
        if(sum == 3) return 1;
        if(sum == -3) return -1;
    }
    // 检查每列
    for(int j = 0; j < 3; j++){
        sum = board[0][j] + board[1][j] + board[2][j];
        if(sum == 3) return 1;
        if(sum == -3) return -1;
    }
    // 主对角线
    sum = board[0][0] + board[1][1] + board[2][2];
    if(sum == 3) return 1;
    if(sum == -3) return -1;
    // 副对角线
    sum = board[0][2] + board[1][1] + board[2][0];
    if(sum == 3) return 1;
    if(sum == -3) return -1;

    return 0;
}
void printWinner(int winner){
    drawGame();
    settextstyle(30, 15, _T("Arial"));
    if(winner == 1){
        outtextxy(GAP, GAP * 4 + 10, _T("O wins!"));
    }else if(winner == -1){
        outtextxy(GAP, GAP * 4 + 10, _T("X wins!"));
    }
    else{
        outtextxy(GAP, GAP * 4 + 10, _T("It's a draw!"));
    }
}
void startGame(){
    srand((unsigned int)time(NULL));
    setbkcolor(BLACK);
    cleardevice();
    setlinecolor(WHITE);
    //重置棋盘状态
    for(int i = 0;i < 3;i++){
        for(int j = 0;j < 3;j++){
            board[i][j] = 0;
        }
    }
    currentPlayer = 1; // O先手
    drawGame();
}

//Q-learning AI
int getQState(){//将当前棋盘状态转换为Q表的状态索引（3进制编码）
    int state = 0;
    for(int i = 0;i < 3;i++){
        for(int j = 0;j < 3;j++){
            if(board[i][j] == 1){
                state = state * 3 + 1;
            }else if(board[i][j] == -1){
                state = state * 3 + 2;
            }else{
                state = state * 3;
            }
        }
    }
    return state;
}
double getQValue(int state, int action){
    return qTable[state][action];
}
int epsilonGreedy(int state){//epsilon-greedy策略选择动作
    double r = (double)rand() / RAND_MAX;
    if(r < epsilon){//探索
        int action;
        //随机选择一个合法动作
        while(1){
            action = rand() % 9;
            int i = action / 3;
            int j = action % 3;
            if(board[i][j] == 0){
                break;
            }
        }
        return action;
    }
    else{
        //利用Q表选择最优动作
        int action = 0;
        double maxQ = -1e9;
        for(int i = 0;i < 9;i++){
            int x = i / 3;
            int y = i % 3;
            if(board[x][y] == 0){
                double qValue = getQValue(state, i);
                if(qValue > maxQ){
                    maxQ = qValue;
                    action = i;
                }
            }
        }
        return action;
    }
}
void makeMove(int action, int player){//根据动作在棋盘上落子
    int i = action / 3;
    int j = action % 3;
    board[i][j] = player;
    currentPlayer *= -1; //切换玩家
}
int getReward(int winner){
    if(winner == 1) return 10; // O赢了
    if(winner == -1) return -10; // X赢了
    return 0; // 平局或未结束
}
void updateQTable(int state, int action, int new_state, int reward) {
    double new_maxQ = -1e9;
    for (int i = 0; i < QY; i++) {
        if (new_maxQ < qTable[new_state][i]) {
            new_maxQ = qTable[new_state][i];
        }
    }
    //Q(s,a) = Q(s,a) + alpha * (reward + gamma_f * maxQ(s') - Q(s,a))
    qTable[state][action] = qTable[state][action] + alpha * (reward + gamma_f * new_maxQ - qTable[state][action]);
}

//文件操作函数
void saveQTable(){
    FILE* file = fopen("qtable.txt", "w");
    for(int i = 0;i < QX;i++){
        for(int j = 0;j < QY;j++){
            fprintf(file, "%lf ", qTable[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}
void loadQTable(){
    FILE* file = fopen("qtable.txt", "r");
    for(int i = 0;i < QX;i++){
        for(int j = 0;j < QY;j++){
            fscanf(file, "%lf", &qTable[i][j]);
        }
    }
    fclose(file);
}

//临时主函数：初始化Q表并保存到文件
// int main(){
//     //初始化Q表
//     for(int i = 0;i < QX;i++){
//         for(int j = 0;j < QY;j++){
//             qTable[i][j] = 0;
//         }
//     }
//     saveQTable("qtable.txt");
//     return 0;
// }


//Q-learning AI训练主函数
int main(){
    initgraph(GAP * 5, GAP * 5);
    BeginBatchDraw();
    for(int i = 0;i < 1000;i++){
        loadQTable();
        for(int episode = 0; episode < 1000; episode++, epsilon *= 1){
            startGame();
            int winner = 0;
            int count = 0;
            while(!winner && count < 9){
                drawGame();
                FlushBatchDraw();
                //获取当前状态,选择动作并执行
                int state = getQState();
                int action = epsilonGreedy(state);
                makeMove(action, currentPlayer);
                //检查胜利条件并更新Q表
                winner = checkWin();
                int reward = getReward(winner);
                int new_state = getQState();
                updateQTable(state, action, new_state, reward);
                count++;
            }
            printWinner(winner);
        }
        saveQTable();
    }
    EndBatchDraw();
    closegraph();
    return 0;
}


//游戏主函数
// int main(){
//     startGame();
//     int winner = 0;
//     int count = 0;
//     while(!winner && count < 9){
//         drawGame();
//         playerMove();
//         winner = checkWin();
//         count++;
//     }
//     printWinner(winner);
//     closegraph();
//     return 0;
// }