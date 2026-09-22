#include <stdio.h>

/*
  练习：
  1. 定义一个函数check_odd_even. 接受3个参数：
  参数一：就是一个整数，
  参数二：一个打印的函数A， 在这个函数里面打印一句话：这个数字是奇数
  参数三：一个打印的函数B， 在这个函数里面打印一句话：这个数字是偶数
  2. 在这个check_odd_even 里面判定传递进来的数字是奇数，就调用函数A，如果是偶数，就调用函数B。
  
  3. 所以练习里面，应该要定义出来3个函数check_odd_even() , printOdd() , printEven()
 */

void check_odd_even(int num ,void(*a)() , void (*b)()){
	if(num %2 != 0){
		a();  // printf("奇数");
	}else{
		b();  // printf("偶数");
	}
}

void printOdd(){
	printf("奇数");
}

void printEven(){
	printf("偶数");
}


int main(){
	check_odd_even(13, printOdd ,printEven);
	
	return 0 ;
}
