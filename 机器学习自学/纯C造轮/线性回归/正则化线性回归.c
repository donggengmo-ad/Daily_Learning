
//大多数代码与多元线性回归相同，区别只出现在反向传播阶段

#include <stdio.h>

// 定义常量
#define FEATURE_NUM 2    // 特征数量（面积+房间数）
#define SAMPLE_NUM 6     // 样本数量
#define epochs 5000
#define lr 0.0001

//正则化超参数
#define lamda_L1 0.01
#define lamda_L2 0.005

//数据定义
double w[FEATURE_NUM] = {0.1,0.1};//权重
double b = 0.1;//偏置 
double sample[SAMPLE_NUM][FEATURE_NUM + 1] = {{50,1,150},//样本数据{面积x1,房间数x2,房价y}
                                            {60,2,180},
                                            {70,2,200},
                                            {80,3,230},
                                            {90,3,250},
                                            {100,4,280}};
//未做归一化，不是重点

//训练
double predict(double *x){
    double y = b;
    for(int i = 0;i < FEATURE_NUM;i++){
        y += w[i] * x[i];
    }
    return y;
}

//符号函数
double sign(double x){
    if(x > 0){
        return 1.0;
    }
    else if(x < 0){
        return -1.0;
    }
    else{
        return 0;
    }
}

void train(){
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
            mse += err * err;
            db += err;
            for(int k = 0;k < FEATURE_NUM;k++){
                dw[k] += err * x[k];
            }
        }
        //反向传播（重点）
        
        for(int j = 0;j < FEATURE_NUM;j++){
            dw[j] *= 2.0 / SAMPLE_NUM;

            /****************************/

            /*
                loss(w,b) = loss(w,b) + 正则化项，整体求偏导数
                L1正则化项 lambda * ∑|wj|
                    偏导数 lamda * ∑ sign(wj)
                L2正则化项 lambda * ∑ wj^2
                    偏导数 lamda * 2 * ∑ wj
            */
            
            //加入L1正则化梯度
            dw[j] += lamda_L1 * sign(w[j]);
            //加入L2正则化梯度
            dw[j] += 2 * lamda_L2 * w[j];

            /****************************/

            w[j] -= lr * dw[j];
        }

        mse /= SAMPLE_NUM;
        db *= 2.0 / SAMPLE_NUM;
        b -= lr * db;
        //每100轮打印一次参数，观察收敛情况
        if(i % 100 == 0){
            printf("第%d轮训练 | MSE=%lf\n",i,mse);
        }
    }
}
void test(){
    double x[FEATURE_NUM],y;
    printf("请输入自变量x的值（%d个）：",FEATURE_NUM);
    for(int i = 0;i < FEATURE_NUM;i++){
        scanf("%lf",&x[i]);
    }
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