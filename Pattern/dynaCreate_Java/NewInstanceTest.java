import java.util.*;
import java.io.*;
import java.lang.*;

public class NewInstanceTest{
    public static void main(String[] args)
    {
        NewInstanceTest t = new NewInstanceTest();
        t.test1();
    }
    public void test1()throws IllegalAccessException,InstantiationException{
        Class clazz = Class.forName("Person");
        //newInstance call not param constructor default and it must be public 
        Person obj = (Person)clazz.newInstance();
        System.out.println(obj);
    }
}