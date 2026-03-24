public class MyProgram {
    public static void main(String args[]) {
        Box b = new Box();
        b.length = 10;
        b.widths = 10;
        b.height = 10;
        System.out.println(b.volume());
    }
}
class Box{
    int length, widths, height;
    public int volume(){
        return length * widths * height;
    }
}

