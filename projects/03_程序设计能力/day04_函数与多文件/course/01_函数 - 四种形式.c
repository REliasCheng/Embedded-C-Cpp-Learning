#include <stdio.h>

//1. 无参无返回值
void sayHi(){
	printf("sayHi::你好~！\n");
}


//2. 有参无返回值 :: 参数个数，和参数类型没有要求。写什么都可以。
// 形式参数 - 形参
void sayHi02(int age , char * name  ){
	printf("sayHi02::你好~！%d岁的%s\n", age , name);
}


// 无参有返回值
int sayHi03(){
	printf("sayHI03::你好~！\n");
	
	//返回具体的数据，数据的类型要和上面标定的返回值类型匹配。
	return 30;
}

// 有参有返回值
int sayHi04(int age , char * name){
	printf("sayHi04::你好~! %d岁的%s\n" , age ,name);
	return 40;
}


int main(){
	
	//调用 ()
	sayHi();
	
	sayHi02(18 , "张三"); // 实际参数 - 实参

	//调用函数，得到结果
	int result = sayHi03();
	printf("返回的结果：%d\n" , result);
	
	int result02 = sayHi04(19 , "李四");
	printf("返回的结果02：%d\n" , result02);
	
	return 0;
}


// 定义计算器的 加减乘除 函数，接受两个数，在函数内部运算得到结果（加法、减法...） 返回结果。
