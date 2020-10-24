import java.lang.reflect.*;
class ClassTest
{
	public static void main(String[] args)
	{
		/*Point pt=new Point();
		Class c1=pt.getClass();
		System.out.println(c1.getName());
		
		try
		{
			Class c2=Class.forName("Point");
			System.out.println(c2.getName());
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		
		Class c3=Point.class;
		System.out.println(c3.getName());
		
		Class c4=int.class;
		System.out.println(c4.getName());
		
		Class c5=Integer.TYPE;
		System.out.println(c5.getName());
		
		Class c6=Integer.class;
		System.out.println(c6.getName());*/
		
		/*System.out.println("before new Point()");
		new Point();
		System.out.println("after new Point()");
		
		try
		{
			Class.forName("Line");
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}*/
		/*if(args.length!=1)
		{
			return;
		}
		try
		{
			Class c=Class.forName(args[0]);
			Point pt=(Point)c.newInstance();
			pt.output();
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}*/
		
		if(args.length!=1)
		{
			return;
		}
		try
		{
			Class c=Class.forName(args[0]);
			Constructor[] cons=c.getDeclaredConstructors();
			/*for(int i=0;i<cons.length;i++)
			{
				System.out.println(cons[i]);
			}
			Method[] ms=c.getDeclaredMethods();
			for(int i=0;i<ms.length;i++)
			{
				System.out.println(ms[i]);
			}*/
			Class[] params=cons[0].getParameterTypes();
			Object[] paramValues=new Object[params.length];
			for(int i=0;i<params.length;i++)
			{
				if(params[i].isPrimitive())
				{
					paramValues[i]=new Integer(i+3);
				}
			}
			Object o=cons[0].newInstance(paramValues);
			Method[] ms=c.getDeclaredMethods();
			ms[0].invoke(o,null);
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
}
class Point
{
	static
	{
		System.out.println("Loading Point");
	}
	int x,y;
	void output()
	{
		System.out.println("x="+x+","+"y="+y);
	}
	Point(int x,int y)
	{
		this.x=x;
		this.y=y;
	}
}
class Line
{
	static
	{
		System.out.println("Loading Line");
	}
}