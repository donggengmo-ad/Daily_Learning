#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
    K 近邻（K-Nearest Neighbors）是最直观的分类算法，核心逻辑是：
    给定一个待分类的样本，计算它与数据集中所有样本的距离（我们用欧氏距离）；
    找出距离最近的 K 个样本（邻居）；
    统计这 K 个邻居的类别，用 “少数服从多数” 的原则确定待分类样本的类别。
    我们会用经典的鸢尾花数据集（简化版）来演示，
    数据集包含 3 类鸢尾花（山鸢尾、变色鸢尾、维吉尼亚鸢尾），
    每个样本有 4 个特征（花萼长度、花萼宽度、花瓣长度、花瓣宽度）。
*/


// 定义常量
#define FEATURE_NUM 4          // 每个样本的特征数（4个）
#define TRAIN_DATA_NUM 14     // 训练集样本数
#define TEST_DATA_NUM 6       // 测试集样本数
#define K 3                    // KNN的K值（选3个最近邻）

// 鸢尾花类别（对应0、1、2）
typedef enum {
    SETOSA,        // 山鸢尾
    VERSICOLOR,    // 变色鸢尾
    VIRGINICA      // 维吉尼亚鸢尾
} IrisType;

// 样本结构体：包含特征和标签
typedef struct {
    float features[FEATURE_NUM];  // 4个特征
    IrisType label;               // 类别标签
} Sample;

// 距离-索引结构体：用于排序邻居
typedef struct {
    float distance;  // 距离
    int index;       // 样本在训练集中的索引
} DistanceIndex;

// 函数声明
void normalize(Sample *data, int data_num);  // 数据归一化
float distance(float *feat1, float *feat2);  // 计算欧氏距离
int compare(const void *a, const void *b);    // qsort排序用的比较函数
IrisType classify(Sample *train_data, float *);  // KNN分类核心
const char* getName(IrisType type);  // 获取鸢尾花名称

// 简化版鸢尾花数据集（训练集+测试集）
// 格式：[花萼长, 花萼宽, 花瓣长, 花瓣宽, 类别]
Sample train_data[TRAIN_DATA_NUM] = {
    {{5.1, 3.5, 1.4, 0.2}, SETOSA}, {{4.9, 3.0, 1.4, 0.2}, SETOSA},
    {{4.7, 3.2, 1.3, 0.2}, SETOSA}, {{4.6, 3.1, 1.5, 0.2}, SETOSA},
    {{5.0, 3.6, 1.4, 0.2}, SETOSA}, {{5.4, 3.9, 1.7, 0.4}, SETOSA},
    {{6.7, 3.1, 4.4, 1.4}, VERSICOLOR}, {{6.9, 3.1, 4.9, 1.5}, VERSICOLOR},
    {{5.5, 2.3, 4.0, 1.3}, VERSICOLOR}, {{6.5, 2.8, 4.6, 1.5}, VERSICOLOR},
    {{7.1, 3.0, 5.9, 2.1}, VIRGINICA}, {{6.3, 2.9, 5.6, 1.8}, VIRGINICA},
    {{6.5, 3.0, 5.8, 2.2}, VIRGINICA}, {{7.6, 3.0, 6.6, 2.1}, VIRGINICA}
};

Sample test_data[TEST_DATA_NUM] = {
    {{5.8, 4.0, 1.2, 0.2}, SETOSA}, {{5.7, 4.4, 1.5, 0.4}, SETOSA},
    {{6.0, 2.2, 4.0, 1.0}, VERSICOLOR}, {{6.1, 2.9, 4.7, 1.4}, VERSICOLOR},
    {{7.7, 2.8, 6.7, 2.0}, VIRGINICA}, {{7.7, 3.8, 6.7, 2.2}, VIRGINICA}
};

int main(){
    // 归一化数据
    normalize(train_data, TRAIN_DATA_NUM); 
    normalize(test_data, TEST_DATA_NUM);  
    // 分类测试集并输出结果
    int correct = 0;
    for (int i = 0; i < TEST_DATA_NUM; i++) {
        //分类样本
        IrisType predict_label = classify(train_data,test_data[i].features);
        IrisType real_label = test_data[i].label;
        //输出结果
         printf("测试样本%d：\n", i+1);
        printf("  真实类别：%s\n", getName(real_label));
        printf("  预测类别：%s\n", getName(predict_label));
        // 统计正确率
        if (predict_label == real_label) {
            correct++;
        }
        printf("------------------------\n");
    } 
    // 输出最终正确率
    float accuracy = (float)correct / TEST_DATA_NUM * 100;
    printf("总正确率：%.2f%% (%d/%d)\n", accuracy, correct, TEST_DATA_NUM);

    return 0;
}

void normalize(Sample *data,int data_num){
    //找出最大最小
    float min[FEATURE_NUM],max[FEATURE_NUM];
    //初始化
    for (int i = 0; i < FEATURE_NUM; i++) {
        min[i] = data[0].features[i];
        max[i] = data[0].features[i];
    }
    //遍历样本
    for(int i = 0;i < data_num;i++){
        for(int j = 0;j < FEATURE_NUM;j++){
            if(max[j] < data[i].features[j]){
                max [j] = data[i].features[j];
            }
            if(min[j] > data[i].features[j]){
                min[j] = data[i].features[j];
            }
        }
    }
    //归一化: x' / 1 = (x-min) / (max-min)
    for(int i = 0;i < data_num;i++){
        for(int j = 0;j < FEATURE_NUM;j++){
            //避免除0
            if(max[j] - min[j] != 0){
                data[i].features[j] = (data[i].features[j] - min[j]) / (max[j] - min[j]);
            }
            else{
                data[i].features[j] = 0;
            }
        }
    }
}
float distance(float *f1,float *f2){
    float sum = 0;
    for(int i = 0;i < FEATURE_NUM;i++){
        float d = f1[i] - f2[i];
        sum += d * d;
    }
    return sqrt(sum);
}
int compare(const void *a, const void *b){
    DistanceIndex *d1 = (DistanceIndex*)a;
    DistanceIndex *d2 = (DistanceIndex*)b;
    if(d1->distance > d2->distance){
        return 1;
    }
    else if(d1->distance < d2->distance){
        return -1;
    }
    else{
        return 0;
    }
}
IrisType classify(Sample *train_data,float *test_feat){
    //记录距离
    DistanceIndex dist[TRAIN_DATA_NUM];
    for(int i = 0;i < TRAIN_DATA_NUM;i++){
        dist[i].distance = distance(test_feat,train_data[i].features);
        dist[i].index = i;
    }
    //排序
    qsort(dist,TRAIN_DATA_NUM,sizeof(DistanceIndex),compare);
    //统计最近邻居
    int count[3] = {0};
    for(int i = 0;i < K;i++){
        int ind = dist[i].index;
        IrisType label = train_data[ind].label;
        count[label]++;
    }
    //找出最多类别
    int max_count = 0;
    IrisType predict = SETOSA;
    for(int i = 0;i < 3;i++){
        if(count[i] > max_count){
            max_count = count[i];
            predict = (IrisType)i;
        }
    }
    return predict;
}
// 获取鸢尾花类别名称（方便输出）
const char* getName(IrisType type) {
    switch (type) {
        case SETOSA: return "山鸢尾(SETOSA)";
        case VERSICOLOR: return "变色鸢尾(VERSICOLOR)";
        case VIRGINICA: return "维吉尼亚鸢尾(VIRGINICA)";
        default: return "未知类别";
    }
}