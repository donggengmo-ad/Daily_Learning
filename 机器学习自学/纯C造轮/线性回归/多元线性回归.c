#include <stdio.h>

// 定义常量
#define FEATURE_NUM 2    // 特征数量（面积+房间数）
#define SAMPLE_NUM 6     // 样本数量

//数据定义
double w[FEATURE_NUM] = {0.1,0.1};//权重
double b = 0.1;//偏置 
double sample[SAMPLE_NUM][FEATURE_NUM + 1] = {{50,1,150},//样本数据{面积x1,房间数x2,房价y}
                                            {60,2,180},
                                            {70,2,200},
                                            {80,3,230},
                                            {90,3,250},
                                            {100,4,280}};
double x1_max,x1_min,x2_max,x2_min;

//预测函数
double predict(double *x){
    double y = b;
    for(int i = 0;i < FEATURE_NUM;i++){
        y += w[i] * x[i];
    }
    return y;
}
//归一化函数
void normalizeSample() {
    // 计算x1、x2的均值和范围
    x1_max = sample[0][0];
    x1_min = sample[0][0];
    x2_max = sample[0][1];
    x2_min = sample[0][1];
    
    // 计算最大最小值
    for(int i=0; i<SAMPLE_NUM; i++) {
        x1_max = (sample[i][0] > x1_max) ? sample[i][0] : x1_max;
        x1_min = (sample[i][0] < x1_min) ? sample[i][0] : x1_min;
        x2_max = (sample[i][1] > x2_max) ? sample[i][1] : x2_max;
        x2_min = (sample[i][1] < x2_min) ? sample[i][1] : x2_min;
    }
    
    // 归一化样本（最小-最大归一化，缩放到0~1）
    for(int i=0; i<SAMPLE_NUM; i++) {
        sample[i][0] = (sample[i][0] - x1_min) / (x1_max - x1_min);
        sample[i][1] = (sample[i][1] - x2_min) / (x2_max - x2_min);
    }
}
void normalizeInput(double x[]){
    // 归一化（最小-最大归一化，缩放到0~1）
    x[0] = (x[0] - x1_min) / (x1_max - x1_min);
    x[1] = (x[1] - x2_min) / (x2_max - x2_min);
}
//训练
void train(){
    normalizeSample();
    int epochs = 5000;
    double lr = 0.001;
    //梯度下降
    for(int i = 0;i < epochs;i++){  
        double dw[FEATURE_NUM] = {0},db = 0;  //参数变化
        double mse = 0;
        //前向传播
        for(int j = 0;j < SAMPLE_NUM;j++){
            //获取样本数据
            double x[FEATURE_NUM] = {0};
            for(int k = 0;k < FEATURE_NUM;k++){
                x[k] = sample[j][k];
            }
            double yr = sample[j][FEATURE_NUM];
            //预测并计算误差
            double yp = predict(x);
            double err = yp -yr;
            //累加
            /* 
                MSE = 1/n * ?∑?(yp - yr?)^2 = 1/n * ?∑?(w*xi ?+ b ? yi?)^2
                dw = ?MSE/?w = 2/n * ∑(yp - yr) * xi
                db = ?MSE/?b = 2/n * ∑(yp - yr)
            */
            db += err;
            mse += err * err;
            for(int k = 0;k < FEATURE_NUM;k++){
                dw[k] += err * x[k];
            }
        }
        //反向传播
        /*
            w = w ? η??L/?w
            b = b ? η??L/?b
        */
        for(int j = 0;j < FEATURE_NUM;j++){
            dw[j] *= 2.0 / SAMPLE_NUM;
            w[j] -= lr * dw[j];
        }
        mse /= SAMPLE_NUM;
        db *= 2.0 / SAMPLE_NUM;
        b -= lr * db;
        //每100轮打印一次参数，观察收敛情况
        if(i % 100 == 0){
            printf("第%d轮训练 | ",i);
            for(int j = 0;j < FEATURE_NUM;j++){
                printf("w%d=%f, ",j+1,w[j]);
            }
            printf("b=%f, MSE=%f\n",b,mse);
        }
    }
}
void test(){
    double x[FEATURE_NUM],y;
    printf("请输入自变量x的值（%d个）：",FEATURE_NUM);
    for(int i = 0;i < FEATURE_NUM;i++){
        scanf("%lf",&x[i]);
    }
    normalizeInput(x);
    y = predict(x);
    printf("当 ");
    for(int i = 0;i < FEATURE_NUM;i++){
        printf("x%d=%.2lf ",i+1,x[i]);
    }
    printf("时，预测值y=%.2lf\n", y);
}
int main(){
    train();
    test();
    return 0;
}