#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
    原始公式：y = 0.8*x1^2 - 0.1*x2^2 + 1.8*x1 + 1.2*x2 + 3
*/

// 定义常量
#define ORIG_FEAT 2 //原始特征
#define FEAT 4 //多项式新特征
#define SAMPLE_NUM 80
// 训练超参数：迭代次数、学习率
#define EPOCHS 30000  // 自行设定（建议30000+，非线性拟合需足够轮数）
#define LR 0.001      // 自行设定（建议0.001~0.01，归一化特征可适当调大）

//数据定义
double w[FEAT] = {0};//权重
double b = 0.1;//偏置 
double sample[SAMPLE_NUM][ORIG_FEAT+1];

//特征映射函数
void featureMap(double x[ORIG_FEAT],double phi[FEAT]){//输入原始特征，输出二阶多项式新特征
    //二6次项
    for(int i = 0;i < ORIG_FEAT;i++){
        phi[i] = x[i] * x[i];
    }
    //一次项
    for(int i = ORIG_FEAT;i < FEAT;i++){
        phi[i] = x[i - ORIG_FEAT];
    }
}
//样本生成函数
void generateSample(){
    srand((unsigned int)time(NULL));
    for(int i = 0;i < SAMPLE_NUM;i++){
        double x[ORIG_FEAT];
        double y;
        //生成x
        for(int j = 0;j < ORIG_FEAT;j++){
            //[-1,1]随机x
            x[j] = (rand() % 200 - 100) / 100.0;
            //同步x到sample
            sample[i][j] = x[j];
        }
        //计算y
        y = 0.8 * x[0] * x[0] - 0.1 * x[1] * x[1] + 1.8 * x[0] + 1.2 * x[1] + 3;
        //添加噪声[-0.1,0.1]
        y += (rand() % 200 - 100) / 1000.0;
        //同步y到sample
        sample[i][ORIG_FEAT] = y;
    }
}
//预测函数
double predict(double x[]){
    double y = b;
    double phi[FEAT];
    featureMap(x,phi);
    for(int i = 0;i < FEAT;i++){
        y += w[i] * phi[i];
    }
    return y;
}
//训练函数
void train(){
    //梯度下降
    for(int i = 0;i < EPOCHS;i++){  
        double dw[FEAT] = {0},db = 0;  //参数变化
        double mse = 0;
        //前向传播
        for(int j = 0;j < SAMPLE_NUM;j++){
            //获取样本数据
            double x[ORIG_FEAT] = {0};
            double phi[FEAT] = {0};
            for(int k = 0;k < ORIG_FEAT;k++){
                x[k] = sample[j][k];
            }
            featureMap(x,phi);
            double yr = sample[j][ORIG_FEAT];
            //预测并计算误差
            double yp = predict(x);
            double err = yp -yr;
            //累加
            /* 
                dw = ?MSE/?w = 2/n * ∑(yp - yr) * xi
                db = ?MSE/?b = 2/n * ∑(yp - yr)
            */
            mse += err * err;
            db += err;
            for(int k = 0;k < FEAT;k++){
                dw[k] += err * phi[k];
            }
        }
        //反向传播
        mse /= SAMPLE_NUM;
        for(int j = 0;j < FEAT;j++){
            dw[j] *= 2.0 / SAMPLE_NUM;
            w[j] -= LR * dw[j];
        }
        db *= 2.0 / SAMPLE_NUM;
        b -= LR * db;
        //每1000轮打印一次参数，观察收敛情况
        if(i % 1000 == 0){
            printf("第%d轮训练 | MSE=%f\n",i,mse);
        }
    }
}
void test(){
    double x[ORIG_FEAT],y;
    printf("请输入自变量x的值（%d个）：",ORIG_FEAT);
    for(int i = 0;i < ORIG_FEAT;i++){
        scanf("%lf",&x[i]);
    }
    y = predict(x);
    printf("当 ");
    for(int i = 0;i < ORIG_FEAT;i++){
        printf("x%d=%.2lf ",i+1,x[i]);
    }
    printf("时，预测值y=%.2lf\n", y);
}
int main(){
    generateSample();
    train();
    test();
    return 0;
}