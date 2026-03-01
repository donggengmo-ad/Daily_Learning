#define SIZE 100
#define GAP 10
#define HEIGHT 4 * SIZE + 2 * GAP
#define WIDTH 4 * SIZE + 2 * GAP

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
} Dir;

extern int block[4][4];
extern int merge_count; // 记录每次移动的合并次数（奖励函数用）

// ========== 神经网络参数配置（新增） ==========
#define INPUT_LAYER_SIZE 16    // 输入层：4x4网格=16个神经元
#define HIDDEN_LAYER_SIZE 32   // 隐藏层神经元数（可调）
#define OUTPUT_LAYER_SIZE 4    // 输出层：上下左右4个方向
#define LEARNING_RATE 0.2f     // 学习率（可调）
#define ACTIVATION_RELU 1      // ReLU激活
#define ACTIVATION_SIGMOID 2   // Sigmoid激活

// 神经网络结构体（存储权重、偏置、各层输出）
typedef struct {
    // 权重矩阵：input->hidden (16×32)
    float w1[INPUT_LAYER_SIZE][HIDDEN_LAYER_SIZE];
    // 权重矩阵：hidden->output (32×4)
    float w2[HIDDEN_LAYER_SIZE][OUTPUT_LAYER_SIZE];
    // 偏置：hidden层 + output层
    float b1[HIDDEN_LAYER_SIZE];
    float b2[OUTPUT_LAYER_SIZE];
    // 各层输出缓存（前向传播用）
    float input[INPUT_LAYER_SIZE];
    float hidden[HIDDEN_LAYER_SIZE];
    float output[OUTPUT_LAYER_SIZE];
    // 反向传播缓存（存储误差）
    float delta_hidden[HIDDEN_LAYER_SIZE];
    float delta_output[OUTPUT_LAYER_SIZE];
} NeuralNetwork;

// ========== 神经网络函数声明（新增） ==========
void nn_init(NeuralNetwork *nn); // 初始化神经网络（随机权重和偏置）
void nn_forward(NeuralNetwork *nn, float *input, int activation_type); // 前向传播
void nn_backward_rl(NeuralNetwork *nn, int action_idx, float reward, float
                    next_value, int activation_type); // 基于奖励的反向传播
int nn_get_best_action(NeuralNetwork *nn); // 获取输出层最大值索引（选动作）
void save_nn(NeuralNetwork *nn); // 保存神经网络参数到文件
void load_nn(NeuralNetwork *nn); // 从文件加载神经网络参数
int calculate_score(int board[4][4]); // 计算当前棋盘的总得分
int count_empty_cells(int board[4][4]); // 计算当前棋盘的空位数
float evaluate_board(int board[4][4], int game_result); // 评估当前棋盘局面（奖励函数）
