#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "2048.h"

/*
神经网络结构：
- 输入层：16个神经元（4x4网格的每个格子一个神经元，输入值为该格子的数值等级，0表示空格）
- 隐藏层：32个神经元（可调，增加容量）
- 输出层：4个神经元（对应4个方向：上、下、左、右，输出值表示选择该方向的概率）
激活函数：
- 隐藏层：ReLU（加快训练，负数归0）或Sigmoid（输出0~1）
- 输出层：Sigmoid（输出0~1概率）
奖励函数设计：
- 失败惩罚：-200（从-1000降低，减少过度惩罚，鼓励探索）
- 胜利奖励：+1000（提升胜利奖励，激励达成目标）
- 得分奖励：直接加上当前棋盘得分（不除以100，保留原始收益）
- 空位奖励：空位数×20（翻倍奖励，鼓励保留操作空间）
- 合并奖励：每合并一次加50分（鼓励合并，提升分数增长）
- 最大数在角落奖励：如果最大数在角落，额外奖励100分（鼓励将大数保存在角落，减少被迫移动的风险）
强化学习算法：
- 使用Q-learning算法，基于奖励的反向传播更新神经网络权重
- 公式：Q值目标 = 即时奖励 + 折扣因子×下一步预估价值（时序差分）
- 反向传播时只更新本次选择的动作对应的权重，其他动作权重不更新，保持稳定学习
*/


// ========== 神经网络基础函数（新增） ==========
// 激活函数：Sigmoid（把数值映射到0~1）
static float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

// Sigmoid导数（反向传播求梯度用）
static float sigmoid_deriv(float x) {// 注意：这里x应该是sigmoid函数的输出值（已经映射到0~1），而不是原始输入
    return x * (1.0f - x);
}

// 激活函数：ReLU（加快训练，负数归0）
static float relu(float x) {
    return x > 0 ? x : 0.0f;
}

// ReLU导数
static float relu_deriv(float x) {
    return x > 0 ? 1.0f : 0.0f;
}

// 初始化神经网络（随机权重、零偏置）
void nn_init(NeuralNetwork *nn) {
    srand((unsigned int)time(NULL));
    // 初始化input->hidden权重（-0.5 ~ 0.5）
    for (int i = 0; i < INPUT_LAYER_SIZE; i++) {
        for (int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
            nn->w1[i][j] = (rand() / (float)RAND_MAX) - 0.5f;
        }
    }
    // 初始化hidden->output权重（-0.5 ~ 0.5）
    for (int i = 0; i < HIDDEN_LAYER_SIZE; i++) {
        for (int j = 0; j < OUTPUT_LAYER_SIZE; j++) {
            nn->w2[i][j] = (rand() / (float)RAND_MAX) - 0.5f;
        }
    }
    // 初始化偏置为0
    memset(nn->b1, 0, sizeof(nn->b1));
    memset(nn->b2, 0, sizeof(nn->b2));
    // 清空缓存
    memset(nn->input, 0, sizeof(nn->input));
    memset(nn->hidden, 0, sizeof(nn->hidden));
    memset(nn->output, 0, sizeof(nn->output));
}
// 前向传播：输入数据通过神经网络得到输出（选方向）
void nn_forward(NeuralNetwork *nn, float *input, int activation_type) {
    // 1. 输入层赋值（把2048的4x4网格数据传入）
    memcpy(nn->input, input, INPUT_LAYER_SIZE * sizeof(float));

    // 2. 计算隐藏层输出（输入×权重 + 偏置 → 激活函数）
    for (int j = 0; j < HIDDEN_LAYER_SIZE; j++) {// 第j个隐藏层神经元
        float sum = nn->b1[j]; // 先加偏置
        for (int i = 0; i < INPUT_LAYER_SIZE; i++) {
            sum += nn->input[i] * nn->w1[i][j]; // 输入×权重累加
        }
        // 激活函数：选ReLU或Sigmoid
        if (activation_type == ACTIVATION_RELU) {
            nn->hidden[j] = relu(sum);//激活完存储在hidden层输出缓存
        } else {
            nn->hidden[j] = sigmoid(sum);
        }
    }

    // 3. 计算输出层输出（隐藏层×权重 + 偏置 → Sigmoid）
    for (int j = 0; j < OUTPUT_LAYER_SIZE; j++) {
        float sum = nn->b2[j];
        for (int i = 0; i < HIDDEN_LAYER_SIZE; i++) {
            sum += nn->hidden[i] * nn->w2[i][j];
        }
        nn->output[j] = sigmoid(sum); // 输出层固定用Sigmoid（0~1概率）
    }
}

// 获取输出层最大值索引（选概率最高的方向：0=UP,1=DOWN,2=LEFT,3=RIGHT）
int nn_get_best_action(NeuralNetwork *nn) {
    int best_idx = 0;
    float max_val = nn->output[0];
    for (int i = 1; i < OUTPUT_LAYER_SIZE; i++) {
        if (nn->output[i] > max_val) {
            max_val = nn->output[i];
            best_idx = i;
        }
    }
    return best_idx;
}

// ========== 新增：2048局面价值评估（奖励函数） ==========
// 1. 计算当前棋盘的总得分（用于评估）
int calculate_score(int board[4][4]) {
    int score = 0;
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (board[x][y] > 0) {
                // 得分 = 2^level（比如level=3 → 8分）
                score += (int)pow(2, board[x][y]);
            }
        }
    }
    return score;
}

// 2. 计算当前棋盘的空位数（空位越多，局面越好）
int count_empty_cells(int board[4][4]) {
    int count = 0;
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (board[x][y] == 0) count++;
        }
    }
    return count;
}

// 3. 评估当前棋盘的总价值（奖励值）
float evaluate_board(int board[4][4], int game_over) {
    float reward = 0.0f;
    
    // 1. 失败惩罚降低，且仅在最终结算时生效
    if (game_over == -1) {
        reward -= 200.0f; // 从-1000→-200
    } else if (game_over == 1) {
        reward += 1000.0f; // 胜利奖励提升
    }
    
    // 2. 得分奖励不除以100，保留原始收益
    reward += calculate_score(board);
    // 3. 空位置奖励翻倍，鼓励保留操作空间
    reward += count_empty_cells(board) * 20.0f;
    
    // 4. 新增“合并奖励”：每合并一次加固定分（需要在moveBlock中统计合并次数）
    // 需在moveBlockTemp中统计合并次数
    reward += merge_count * 50.0f;
    merge_count = 0; // 用完就清零，准备下一次统计
    
    // 5. 最大数在角落的奖励提升
    int max_val = 0, max_x = 0, max_y = 0;
    for (int x = 0; x < 4; x++) for (int y = 0; y < 4; y++) {
        if (board[x][y] > max_val) { max_val = board[x][y]; max_x = x; max_y = y; }
    }
    if ((max_x == 0 && max_y == 0) || (max_x == 0 && max_y == 3) ||
        (max_x == 3 && max_y == 0) || (max_x == 3 && max_y == 3)) {
        reward += 100.0f;
    }
    
    return reward;
}

// ========== 改造：基于奖励的反向传播（试错学习） ==========
// 参数说明：
// nn: 神经网络
// action_idx: AI本次选择的动作（0=UP,1=DOWN,2=LEFT,3=RIGHT）
// reward: 本次动作的奖励值
// next_value: 下一步局面的预估价值（时序差分）
// activation_type: 激活函数类型
void nn_backward_rl(NeuralNetwork *nn, int action_idx, float reward, float next_value, int activation_type) {
    // 强化学习核心公式：Q值目标 = 即时奖励 + 折扣因子×下一步预估价值
    // Q(s,a) = Q(s,a) + alpha * (reward + gamma * maxQ(s') - Q(s,a))
    float gamma = 0.9f; // 折扣因子（未来奖励的权重，0~1）
    float target_q = reward + gamma * next_value;
    
    // 1. 计算输出层误差（目标Q值 - 实际输出Q值）× Sigmoid导数
    float current_q = nn->output[action_idx]; // AI本次动作的预估Q值
    float error = target_q - current_q;
    nn->delta_output[action_idx] = error * sigmoid_deriv(current_q);
    // 其他动作的误差设为0（只更新本次选择的动作）
    for (int j = 0; j < OUTPUT_LAYER_SIZE; j++) {
        if (j != action_idx) {
            nn->delta_output[j] = 0.0f;
        }
    }

    // 2. 计算隐藏层误差（和原有逻辑一致，但只受本次动作误差影响）
    for (int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
        float err = 0.0f;
        for (int k = 0; k < OUTPUT_LAYER_SIZE; k++) {
            err += nn->delta_output[k] * nn->w2[j][k];
        }
        if (activation_type == ACTIVATION_RELU) {
            nn->delta_hidden[j] = err * relu_deriv(nn->hidden[j]);
        } else {
            nn->delta_hidden[j] = err * sigmoid_deriv(nn->hidden[j]);
        }
    }

    // 3. 更新hidden->output权重（只更新本次动作对应的权重）
    for (int i = 0; i < HIDDEN_LAYER_SIZE; i++) {
        nn->w2[i][action_idx] += LEARNING_RATE * nn->delta_output[action_idx] * nn->hidden[i];
        // 其他动作权重不更新
    }

    // 4. 更新input->hidden权重（所有权重都更新，因为隐藏层影响所有输出）
    for (int i = 0; i < INPUT_LAYER_SIZE; i++) {
        for (int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
            nn->w1[i][j] += LEARNING_RATE * nn->delta_hidden[j] * nn->input[i];
        }
    }

    // 5. 更新偏置（只更新本次动作对应的输出层偏置）
    for (int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
        nn->b1[j] += LEARNING_RATE * nn->delta_hidden[j];
    }
    nn->b2[action_idx] += LEARNING_RATE * nn->delta_output[action_idx];
    // 其他动作偏置不更新
}
void save_nn(NeuralNetwork *nn) {
    FILE *file = fopen("nn_model.txt", "w");
    if (file) {
        //写入input->hidden权重
        for(int i = 0; i < INPUT_LAYER_SIZE; i++) {
            for(int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
                fprintf(file,"%f ",nn->w1[i][j]);
            }
            fprintf(file,"\n");
        }
        //写入偏置
        for(int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
            fprintf(file,"%f ",nn->b1[j]);
        }
        fprintf(file,"\n");
        //写入hidden->output权重
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++) {
            for(int j = 0; j < OUTPUT_LAYER_SIZE; j++) {
                fprintf(file,"%f ",nn->w2[i][j]);
            }
            fprintf(file,"\n");
        }
        for(int j = 0; j < OUTPUT_LAYER_SIZE; j++) {
            fprintf(file,"%f ",nn->b2[j]);
        }
        fclose(file);
    }
}
void load_nn(NeuralNetwork *nn) {
    FILE *file = fopen("nn_model.txt", "r");
    if (file) {
        for(int i = 0; i < INPUT_LAYER_SIZE; i++) {
            for(int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
                fscanf(file,"%f",&nn->w1[i][j]);
            }
        }
        for(int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
            fscanf(file,"%f",&nn->b1[j]);
        }
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++) {
            for(int j = 0; j < OUTPUT_LAYER_SIZE; j++) {
                fscanf(file,"%f",&nn->w2[i][j]);
            }
        }
        for(int j = 0; j < OUTPUT_LAYER_SIZE; j++) {
            fscanf(file,"%f",&nn->b2[j]);
        }
        fclose(file);
    }
}