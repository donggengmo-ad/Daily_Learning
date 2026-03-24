#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <graphics.h>	

//算法
int fac(int n){//阶乘
    if(n == 0 || n == 1){
        return 1;
    }
    else{
        return n * fac(n - 1);
    }
}
int add(int x,int y){
    return x + y;
}
void sortup(int value[],int n){//从小到大排序
    int i,j,temp;
    for(i = n - 1;i >= 1;i--){
        for(j = 0;j < i;j++){
            if(value[j] > value[j + 1]){
                temp = value[j];
                value[j] = value[j + 1];
                value[j + 1] = temp;
            }
        }
    }
}
void sortdn(int value[],int n){//从大到小排序
    int i,j,temp;
    for(i = n - 1;i >= 1;i--){
        for(j = 0;j < i;j++){
            if(value[j] < value[j + 1]){
                temp = value[j];
                value[j] = value[j + 1];
                value[j + 1] = temp;
            }
        }
    }
}
void input(int value[],int n){//数组批量输入
    int i;
    for(i = 0;i < n;i++){
        scanf("%d",&value[i]);
    }
} 
void output(int value[],int n){//数组批量输出
    int i;
    for(i = 0;i < n;i++){
        printf("%d",value[i]);
    }
} 
void swap(int *p,int *q){
    int temp;
    temp = *q;
    *q = *p;
    *p = temp;
}
int A(int m,int n){//计算排列数（需要阶乘函数）
    int x;
    x = (fac(n))/(fac(n-m));
    return x;
}
int C(int m,int n){//计算组合数（需要阶乘函数）
    int x;
    x = (fac(n))/((fac(m))*(fac(n-m)));
    return x;
}
int search(int value[],int key,int high,int low){//从数组中查找某数（二分查找）
    int mid;
    if(low > high){
        return -1;
    }
    mid = (high + low)/2;
    if(mid == key){
        return mid;
    }
    else if(value[mid] > key){
        return search(value,key,mid - 1,low);
    }
    else{
        return search(value,key,high,mid + 1);
    }
}
double dist(int x1,int y1,int x2,int y2){//计算两点距离
    double result;
    result = sqrt(pow((x1 - x2),2) + pow((y1 - y2),2));
    return result;
}
void shuffleArray(int arr[], int size) {// 洗牌函数：打乱数组顺序（Fisher-Yates 算法，高效公平）
    for (int i = size - 1; i > 0; i--) {
        // 生成0~i之间的随机索引
        int j = rand() % (i + 1);
        // 交换arr[i]和arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}
//控制台
void gotoxy(int x,int y){//移动光标到xy位置
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(handle,pos);
}
void HideCursor(){//隐藏光标
    CONSOLE_CURSOR_INFO cursor_info = {1,0};//第二个值为0表示隐藏光标
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}
//EasyX图形库
void transparentimage(IMAGE* picture, int  picture_x, int picture_y){ //png透明图片
	// 变量初始化
	DWORD *dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD *draw = GetImageBuffer();
	DWORD *src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标
 
	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}
void PutImageZoom(int x, int y, int w, int h, IMAGE* srcImg){//缩放图片大小后输出
	// 创建目标尺寸的兼容DC用于缩放操作
	IMAGE destImg(w,h);
	// 设置缩放模式为高质量
	SetStretchBltMode(GetImageHDC(&destImg), HALFTONE);
	// 使用StretchBlt进行高质量缩放
	StretchBlt(GetImageHDC(&destImg), 0, 0, w, h, GetImageHDC(srcImg), 0, 0, srcImg->getwidth(), srcImg->getheight(), SRCCOPY);
	putimage(x, y, &destImg);
	SetStretchBltMode(GetImageHDC(&destImg), COLORONCOLOR);// 恢复默认缩放模式
}
void outtextxywrap(int x, int y, const TCHAR* str, int lineHeight) {// 自动解析 TCHAR 字符串中的 \n，实现换行输出
    // 参数：x/y - 起始坐标；str - 含 \n 的 TCHAR 字符串；lineHeight - 行高（建议字号+10）
    if (str == NULL || lineHeight <= 0) return;

    TCHAR buf[1024] = {0}; // 临时缓冲区，存储单行内容
    const TCHAR* p = str;  // 遍历字符串的指针
    int bufIdx = 0;        // 缓冲区索引

    while (*p != _T('\0')) { // 遍历到字符串结束
        if (*p == _T('\n')) { // 遇到换行符，输出当前行
            buf[bufIdx] = _T('\0'); // 终止单行字符串
            outtextxy(x, y, buf);
            y += lineHeight;        // 换行：Y 坐标增加
            bufIdx = 0;             // 重置缓冲区索引
        } else { // 非换行符，存入缓冲区
            buf[bufIdx++] = *p;
        }
        p++;
    }

    // 输出最后一行（无 \n 结尾的剩余内容）
    if (bufIdx > 0) {
        buf[bufIdx] = _T('\0');
        outtextxy(x, y, buf);
    }
}

int main(){
    int n;
    scanf("%d",&n);
    printf("%d",fac(n));
    return 0;
}