public class 类创建 {
    public static void main(String[] args) {}
}
class Fraction {
    private int num, den;

    //构造方法
    Fraction(int num, int den) {
        //this指代当前对象
        this.num = num;
        this.den = den;
    }
    //无参
    Fraction() {
        //this方法调用其他构造方法
        this(0,0);
    }
    //规范：成员有getter和setter方法
    public int getNum() {
        return num;
    }
    public void setNum(int num) {
        this.num = num;
    }
    public int getDen() {
        return den;
    }
    public void setDen(int den) {
        this.den = den;
    }
    //Override方法
    @Override
    public String toString() {
        return num + "/" + den;
    }
    @Override
    public boolean equals(Object obj) {
        //自己比自己
        if(obj == this)  return true;
        //空或不同类
        if(obj == null || obj.getClass() != this.getClass()) return false;
        //类型转换
        Fraction other = (Fraction) obj;
        //比较关键字段
        return this.num == other.num && this.den == other.den;
    }
}
