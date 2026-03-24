public class Fibonacci {
    public static void main(String[] args){
        int[] fib = new int [30];
        fibCreate(fib);
        fibDisplay(fib);

        System.out.println();

        fibon fi = new fibon();
        fi.create();
        fi.display();
    }
    //静态方法
    static void fibCreate(int[] fib){
        fib[0] = 0;
        fib[1] = 1;
        for (int i = 2;i < fib.length;i++){
            fib[i] = fib[i-1] + fib[i-2];
        }
    }
    static void fibDisplay(int[] fib){
        for(int i:fib){
            System.out.print(i+" ");
        }
    }
}
//对象封装
class fibon {
    public int[] array;
    public void create(){
        this.array = new int[30];
        this.array[0] = 0;
        this.array[1] = 1;
        for (int i = 2;i < this.array.length;i++){
            this.array[i] = this.array[i-1] + this.array[i-2];
        }
    }
    public void display(){
        for(int i:this.array){
            System.out.print(i+" ");
        }
    }
}