#include <stdio.h>

//数据定义
double w = 0.1;//权重
double b = 0.1;//偏置 
double sample[6][2] = {{50,150},//样本数据{面积x,房价y}
                       {60,180},
                       {70,200},
                       {80,230},
                       {90,250},
                       {100,280}};

//训练过程
double predict(double x){//预测函数
    double y = w * x + b;
    return y;
}
void train(){
    int epochs = 2000;//迭代次数
    double lr = 0.0001;//learning-rate学习率
    int n = sizeof(sample)/sizeof(sample[0]); // 自动计算样本数
    //梯度下降训练
    for(int i = 0;i < epochs;i++){
        double dw = 0,db = 0;//参数变化
        //前向传播
        for(int j = 0;j < n;j++){//遍历所有样本
            //获取样本数据
            double x = sample[j][0];//输入
            double yr = sample[j][1];//真实值
            //预测并记录误差
            double yp = predict(x);//预测值
            double err = yp - yr;//误差
            //记录变化
            /* 
                w = w ? η??L/?w
                b = b ? η??L/?b
                平均方差误差MSE(Mean Squared Error)
                MSE = 1/n * ?∑?(yp - yr?)^2 = 1/n * ?∑?(w*xi ?+ b ? yi?)^2
                dw = ?MSE/?w = 2/n * ∑(yp - yr) * xi
                db = ?MSE/?b = 2/n * ∑(yp - yr)
            */
            dw += x * err;
            db += err;
        }
        //反向传播
        dw *= 2.0 / n;
        db *= 2.0 / n;
        w -= lr * dw;
        b -= lr * db;
        //每100轮打印一次参数，观察收敛情况
        if(i % 100 == 0){
            printf("第%d轮训练 | w=%f, b=%f\n", i, w, b);
        }
    }
}
void test(){
    double x,y;
    printf("请输入自变量x的值：");
    scanf("%lf",&x);
    y = predict(x);
    printf("当x=%.2lf时，预测值y=%.2lf\n", x, y);
}
int main(){
    train();
    test();
    return 0;
}