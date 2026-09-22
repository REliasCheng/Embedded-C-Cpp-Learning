#include <stdio.h>

/*
  
  1. C语言的函数在传参的时候，传递的参数，其实是参数的拷贝，或者说是原始数据的一个副本。
  2. 在函数内部操作这份数据，其实不会对外面的原始数据产生影响！
  
 */

// 定义一个函数，然后交换两个整数变量的值

void exchange(int a ,  int b){
	printf("交换前:: a=%d , b=%d \n" , a ,b);
	
	//把a的值，丢给temp
	int temp = a;
	
	//把b的值, 丢给a
	a = b;
	
	//把temp的值，丢给b
	b = temp;
	
	printf("交换前:: a=%d , b=%d \n" , a ,b);
}

int main(){
	
	//1. 定义变量
	int num1 = 20;
	int num2 = 30;
	
	printf("交换前:: num1=%d , num2=%d \n" , num1 ,num2);
	
	//2. 调用函数，完成交换
	exchange(num1 , num2);
	
	//3. 打印结果
	printf("交换后:: num1=%d , num2=%d \n" , num1 ,num2);
	
	
	return 0 ;
}
