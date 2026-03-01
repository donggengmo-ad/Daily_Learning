#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <conio.h>
#include <tchar.h>
#include <string.h>
#include "2048.h"

int block[4][4];
int merge_count; // 记录每次移动的合并次数（奖励函数用）

void drawBlock(int x0,int y0,int level){  
    //根据数字设置颜色
    int hue = level * 360 / 11;
    int color = HSLtoRGB(hue,1,0.5);
    //计算方块位置
    int x = x0 * SIZE + GAP;
    int y = y0 * SIZE + GAP;
    //绘制方块
    if(level == 0){
        //空格为浅灰色
        color = LIGHTGRAY; //空格为浅灰色
        setfillcolor(color);
        setlinecolor(color);
        fillrectangle(x,y,x+SIZE,y+SIZE);
    }
    else{
        //非空格为对应颜色
        setfillcolor(color);
        setlinecolor(BLACK);
        fillrectangle(x,y,x+SIZE,y+SIZE);
        //绘制数字
        TCHAR str[10];
        _itot(pow(2,level),str,10);  
        int len = _tcslen(str) * SIZE/4;
        int tx = x + SIZE / 2 - len / 2;
        int ty = y + SIZE / 2 - SIZE / 4;
        outtextxy(tx,ty,str);
    }
}
void addBlock(int num){
    int emptyCount = 0;
    // 先统计空位数量，避免死循环
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if(block[i][j] == 0) emptyCount++;
        }
    }
    // 如果没有空位，直接返回
    if(emptyCount == 0) return;
    // 随机选择一个空位
    int temp = rand() % 16;
    for(int i = 0;i < num;i++){
        if(emptyCount <= i) break; // 如果空位不足，提前结束
        while(block[temp % 4][temp / 4] != 0){
            temp = rand() % 16;
        }
        int x = temp % 4;
        int y = temp / 4;
        block[x][y] = rand() % 2 + 1; //随机生成1或2
        temp = rand() % 16;
    }
}
void startGame(){
    //初始化游戏界面
    initgraph(WIDTH, HEIGHT);
    BeginBatchDraw();
    setbkcolor(WHITE);
    setbkmode(TRANSPARENT);
    settextstyle(SIZE/2,SIZE/4,_T("Arial"));
    settextcolor(BLACK);
    cleardevice();
    //初始化游戏数据
    srand((unsigned int)time(NULL));
    for(int i = 0;i < 4;i++){
        for(int j = 0;j < 4;j++){
            block[i][j] = 0;
        }
    }
    //添加初始数字
    addBlock(2);
}
void drawGame(){
    cleardevice();
    //绘制游戏界面
    for(int y = 0;y < 4;y++){
        for(int x = 0;x < 4;x++){
            drawBlock(x,y,block[x][y]);
        }
    }
    FlushBatchDraw();
}
Dir input(){
    Dir dir = NONE;
    ExMessage msg;
    peekmessage(&msg,EM_KEY);
    if(msg.message == WM_KEYDOWN){
        //根据按键更新游戏数据
        switch(msg.vkcode){
            case VK_LEFT:
                //向左移动
                dir = LEFT;
                break;
            case VK_RIGHT:
                //向右移动
                dir = RIGHT;
                break;
            case VK_UP:
                //向上移动
                dir = UP;
                break;
            case VK_DOWN:
                //向下移动
                dir = DOWN;
                break;
        }
    }
    return dir;
}
int moveBlockTemp(int tempBlock[4][4], Dir dir){
    int moved = 0;
    if(dir == NONE) return moved;

    if(dir == LEFT){
        for(int y = 0;y < 4;y++){
            // 步骤1：将非0方块左移到最左侧（无合并）
            int newCol[4] = {0};
            int idx = 0;
            for(int x = 0;x < 4;x++){
                if(tempBlock[x][y] != 0){
                    newCol[idx++] = tempBlock[x][y];
                }
            }
            // 步骤2：合并相同方块
            idx = 0;
            while(idx < 3){
                if(newCol[idx] != 0 && newCol[idx] == newCol[idx+1]){
                    newCol[idx]++;
                    newCol[idx+1] = 0;
                    moved = 1;
                    idx += 2; // 合并后跳过下一个（避免重复合并）
                    merge_count++;
                } else {
                    idx++;
                }
            }
            // 步骤3：重新整理（将合并后的0移到右侧）
            int finalCol[4] = {0};
            idx = 0;
            for(int x = 0;x < 4;x++){
                if(newCol[x] != 0){
                    finalCol[idx++] = newCol[x];
                }
            }
            // 步骤4：更新临时数组
            for(int x = 0;x < 4;x++){
                if(tempBlock[x][y] != finalCol[x]){
                    moved = 1;
                }
                tempBlock[x][y] = finalCol[x];
            }
        }
    } else if(dir == RIGHT){
        for(int y = 0;y < 4;y++){
            int newCol[4] = {0};
            int idx = 3;
            for(int x = 3;x >= 0;x--){
                if(tempBlock[x][y] != 0){
                    newCol[idx--] = tempBlock[x][y];
                }
            }
            idx = 3;
            while(idx > 0){
                if(newCol[idx] != 0 && newCol[idx] == newCol[idx-1]){
                    newCol[idx]++;
                    newCol[idx-1] = 0;
                    moved = 1;
                    idx -= 2;
                    merge_count++;
                } else {
                    idx--;
                }
            }
            int finalCol[4] = {0};
            idx = 3;
            for(int x = 3;x >= 0;x--){
                if(newCol[x] != 0){
                    finalCol[idx--] = newCol[x];
                }
            }
            for(int x = 0;x < 4;x++){
                if(tempBlock[x][y] != finalCol[x]){
                    moved = 1;
                }
                tempBlock[x][y] = finalCol[x];
            }
        }
    } else if(dir == UP){
        for(int x = 0;x < 4;x++){
            int newRow[4] = {0};
            int idx = 0;
            for(int y = 0;y < 4;y++){
                if(tempBlock[x][y] != 0){
                    newRow[idx++] = tempBlock[x][y];
                }
            }
            idx = 0;
            while(idx < 3){
                if(newRow[idx] != 0 && newRow[idx] == newRow[idx+1]){
                    newRow[idx]++;
                    newRow[idx+1] = 0;
                    moved = 1;
                    idx += 2;
                    merge_count++;
                } else {
                    idx++;
                }
            }
            int finalRow[4] = {0};
            idx = 0;
            for(int y = 0;y < 4;y++){
                if(newRow[y] != 0){
                    finalRow[idx++] = newRow[y];
                }
            }
            for(int y = 0;y < 4;y++){
                if(tempBlock[x][y] != finalRow[y]){
                    moved = 1;
                }
                tempBlock[x][y] = finalRow[y];
            }
        }
    } else if(dir == DOWN){
        for(int x = 0;x < 4;x++){
            int newRow[4] = {0};
            int idx = 3;
            for(int y = 3;y >= 0;y--){
                if(tempBlock[x][y] != 0){
                    newRow[idx--] = tempBlock[x][y];
                }
            }
            idx = 3;
            while(idx > 0){
                if(newRow[idx] != 0 && newRow[idx] == newRow[idx-1]){
                    newRow[idx]++;
                    newRow[idx-1] = 0;
                    moved = 1;
                    idx -= 2;
                    merge_count++;
                } else {
                    idx--;
                }
            }
            int finalRow[4] = {0};
            idx = 3;
            for(int y = 3;y >= 0;y--){
                if(newRow[y] != 0){
                    finalRow[idx--] = newRow[y];
                }
            }
            for(int y = 0;y < 4;y++){
                if(tempBlock[x][y] != finalRow[y]){
                    moved = 1;
                }
                tempBlock[x][y] = finalRow[y];
            }
        }
    }
    return moved;
}
int moveBlock(Dir dir){
    int tempBlock[4][4];
    memcpy(tempBlock, block, sizeof(block));
    int moved = moveBlockTemp(tempBlock, dir);
    if(moved){
        // 如果移动有效，更新全局数组
        memcpy(block, tempBlock, sizeof(block));
    }
    return moved;
}
int checkOver(){
    // 检测是否达到2048（胜利）
    for(int x = 0; x < 4; x++){
        for(int y = 0; y < 4; y++){
            if(block[x][y] == 11){ // 2^11 = 2048
                return 1;
            }
        }
    }

    // 更可靠的失败检测：对每个方向在临时数组上模拟一次移动，如果任何方向能移动或合并，则游戏继续。
    for(int d = UP; d < NONE; d++){
        int tempBlock[4][4];
        memcpy(tempBlock, block, sizeof(tempBlock));
        if(moveBlockTemp(tempBlock, (Dir)d)){
            return 0; // 有可移动或可合并的情况，游戏继续
        }
    }

    // 无任何方向能移动 -> 失败
    return -1;
}
void update(Dir dir){
    //移动方块
    int moved = moveBlock(dir);
    //随机添加新数字
    if(moved){
        addBlock(1);
    }
}
void endGame(int result){
    drawGame();
    //显示结果
    settextstyle(SIZE/4,SIZE/8,_T("Calibri"));
    settextcolor(BLACK);
    if(result == 1){
        //成功界面
        outtextxy(10,100,_T("You Win!"));
    }
    else if (result == -1) {
        //失败界面
        outtextxy(10,100,_T("Game Over!"));
    }
    outtextxy(10,10,_T("Press any key to exit"));
    FlushBatchDraw();
    // 显示至少 2 秒以确保玩家看到结算界面，然后自动退出
    Sleep(2500);
    //结束游戏界面
    EndBatchDraw();
    closegraph();
    // 退出
}

int main0(){
    startGame();
    int result;
    while(1){
        result = checkOver();
        if(result != 0){
            break;
        }
        Dir dir = input();
        update(dir);
        drawGame();
    }
    endGame(result);
    return 0;
}

int main(){
    startGame();
    int result;
    
    // 初始化神经网络
    NeuralNetwork nn;
    nn_init(&nn);
    load_nn(&nn); // 尝试加载已有的训练模型（如果有）
    for(int episode = 0; episode < 10000; episode++) { // 训练多个回合
        // 每个回合开始时重置游戏
        for(int i = 0;i < 4;i++){
            for(int j = 0;j < 4;j++){
                block[i][j] = 0;
            }
        }
        addBlock(2); // 添加初始方块

        // 保存上一步的棋盘价值（用于时序差分）
        float prev_value = 0.0f;
        
        while(1){
            result = checkOver();
            if(result != 0){
                break; // 游戏结束
            }
            
            // ========== 1. 构造输入，AI选动作 ==========
            float nn_input[INPUT_LAYER_SIZE];
            int idx = 0;
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    nn_input[idx++] = block[x][y] / 11.0f;
                }
            }
            // 前向传播：获取4个方向的Q值（预估奖励）
            nn_forward(&nn, nn_input, ACTIVATION_RELU);
            // 选Q值最高的动作
            // 选择动作时，加入ε随机探索（ε随训练轮数递减）
            float epsilon = 0.5f - (episode / 2000.0f); // 从0.5降到0
            epsilon = fmax(epsilon, 0.1f); // 保留最小探索率
            int action;
            Dir dir = NONE;
            if (rand() / (float)RAND_MAX < epsilon) {
                // 随机选有效动作
                Dir valid_dirs[4];
                int valid_count = 0;
                for (int d = UP; d < NONE; d++) {
                    int temp_block[4][4];
                    memcpy(temp_block, block, sizeof(temp_block));
                    if (moveBlockTemp(temp_block, (Dir)d)) {
                        valid_dirs[valid_count++] = (Dir)d;
                    }
                }
                if (valid_count > 0) {
                    action = rand() % valid_count;
                    dir = valid_dirs[action];
                }
            } else {
                // 贪心选最大Q值动作
                nn_forward(&nn, nn_input, ACTIVATION_RELU);
                action = nn_get_best_action(&nn);
                // ... 原有动作转换逻辑
                switch(action) {
                    case 0: dir = UP; break;
                    case 1: dir = DOWN; break;
                    case 2: dir = LEFT; break;
                    case 3: dir = RIGHT; break;
                }
            }
            int temp[4][4];
            memcpy(temp, block, sizeof(temp));
            int moved = moveBlockTemp(temp,dir);
            if(!moved){
                // 如果AI选的动作无效（无法移动），随机选择一个有效动作
                Dir valid_dirs[4];
                int valid_count = 0;
                for (int d = UP; d < NONE; d++) {
                    int temp_block[4][4];
                    memcpy(temp_block, block, sizeof(temp_block));
                    if (moveBlockTemp(temp_block, (Dir)d)) {
                        valid_dirs[valid_count++] = (Dir)d;
                    }
                }
                if (valid_count > 0) {
                    dir = valid_dirs[rand() % valid_count];
                } 
                else {
                    dir = NONE; // 无有效动作，游戏结束
                }
            }
            
            
            // ========== 2. 保存移动前的状态（用于计算奖励） ==========
            int pre_board[4][4];
            memcpy(pre_board, block, sizeof(pre_board));
            int pre_empty = count_empty_cells(pre_board);
            
            // ========== 3. 执行移动，更新游戏 ==========
            moved = moveBlock(dir); // 直接调用moveBlock，替代原有update
            if (moved) {
                addBlock(1); // 移动有效才加新方块
            }
            drawGame();
            
            // ========== 4. 计算本次动作的奖励和学习 ==========
            // 4.1 计算即时奖励
            float reward = 0.0f;
            // 惩罚：移动无效（比如按左但无法左移）
            if (!moved) {
                reward -= 50.0f;
            } 
            else {
                // 奖励：移动后的局面价值
                reward = evaluate_board(block, result);
                // 额外奖励：合并出更大的数（对比移动前后得分）
                int pre_score = calculate_score(pre_board);
                int cur_score = calculate_score(block);
                if (cur_score > pre_score) {
                    reward += (cur_score - pre_score) / 10.0f;
                }
            }
            
            // 4.2 计算下一步的预估价值（时序差分）
            // 构造下一步的输入，前向传播获取最大Q值（未来预估价值）
            float next_input[INPUT_LAYER_SIZE];
            idx = 0;
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    next_input[idx++] = block[x][y] / 11.0f;
                }
            }
            nn_forward(&nn, next_input, ACTIVATION_RELU);
            // 下一步的最大Q值（预估未来奖励）
            // 在计算next_value前加判断
            float next_value = 0.0f;
            if (result == 0) { // 游戏未结束时才计算下一步Q值
                nn_forward(&nn, next_input, ACTIVATION_RELU);
                for (int i = 0; i < OUTPUT_LAYER_SIZE; i++) {
                    if (nn.output[i] > next_value) next_value = nn.output[i];
                }
            }
            
            // 4.3 反向传播：用奖励值更新神经网络（试错学习）
            // 重新前向传播一次（恢复本次动作的Q值计算）
            nn_forward(&nn, nn_input, ACTIVATION_RELU);
            nn_backward_rl(&nn, action, reward, next_value, ACTIVATION_RELU);
            
            // ========== 5. 延时，方便观察 ==========
        }
        
        // 游戏结束：给最终惩罚/奖励
        float final_reward = evaluate_board(block, result);
        printf("游戏结束，最终奖励值：%.2f\n", final_reward);
        save_nn(&nn); // 保存训练好的神经网络参数（可选）
    }
    endGame(result);
    return 0;
}

int main1(){
    startGame();
    int result;
    
    // 初始化神经网络
    NeuralNetwork nn;
    nn_init(&nn);
    load_nn(&nn); // 尝试加载已有的训练模型（如果有）
        while(1){
            result = checkOver();
            if(result != 0){
                break; // 游戏结束
            }
            
            // ========== 1. 构造输入，AI选动作 ==========
            float nn_input[INPUT_LAYER_SIZE];
            int idx = 0;
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    nn_input[idx++] = block[x][y] / 11.0f;
                }
            }
            // 前向传播：获取4个方向的Q值（预估奖励）
            nn_forward(&nn, nn_input, ACTIVATION_RELU);
            // 选Q值最高的动作
            Dir dir = NONE;
            nn_forward(&nn, nn_input, ACTIVATION_RELU);
            int action = nn_get_best_action(&nn);
            // ... 原有动作转换逻辑
            switch(action) {
                case 0: dir = UP; break;
                case 1: dir = DOWN; break;
                case 2: dir = LEFT; break;
                case 3: dir = RIGHT; break;
            }
            int temp[4][4];
            memcpy(temp, block, sizeof(temp));
            int moved = moveBlockTemp(temp,dir);
            if(!moved){
                // 如果AI选的动作无效（无法移动），随机选择一个有效动作
                Dir valid_dirs[4];
                int valid_count = 0;
                for (int d = UP; d < NONE; d++) {
                    int temp_block[4][4];
                    memcpy(temp_block, block, sizeof(temp_block));
                    if (moveBlockTemp(temp_block, (Dir)d)) {
                        valid_dirs[valid_count++] = (Dir)d;
                    }
                }
                if (valid_count > 0) {
                    dir = valid_dirs[rand() % valid_count];
                } 
                else {
                    dir = NONE; // 无有效动作，游戏结束
                }
            }
            
            
            // ========== 2. 保存移动前的状态（用于计算奖励） ==========
            int pre_board[4][4];
            memcpy(pre_board, block, sizeof(pre_board));
            int pre_empty = count_empty_cells(pre_board);
            
            // ========== 3. 执行移动，更新游戏 ==========
            moved = moveBlock(dir); // 直接调用moveBlock，替代原有update
            if (moved) {
                addBlock(1); // 移动有效才加新方块
            }
            drawGame();
            
            // ========== 5. 延时，方便观察 ==========
            Sleep(100);
        }
        
        // 游戏结束：给最终惩罚/奖励
        float final_reward = evaluate_board(block, result);
        printf("游戏结束，最终奖励值：%.2f\n", final_reward);
        save_nn(&nn); // 保存训练好的神经网络参数（可选）
    endGame(result);
    return 0;
}