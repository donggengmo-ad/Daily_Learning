import java.util.Scanner;
public class 输入输出 {
    public void main(){
        System.out.println("你好"+"。");
        System.out.println("\t\t啦啦啦啦");
        System.out.println(3/5.0 + " " + 5%3);

        Scanner scn = new Scanner(System.in);
        String word = scn.next();
        System.out.print(word);//不带换行
        scn.nextLine();//吞掉换行
        String line = scn.nextLine();
        System.out.println(line);
    }
}
