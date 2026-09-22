#include <stdio.h>

//需求：定义一个数组，然后把数组传递给函数，在函数内部打印这个数组

//数组当成参数来传递的时候，传递的其实是首元素的地址，既然是地址，那么就可以定义参数为指针！
//void printArr(int arr[5]){
//void printArr(int arr[]){
void printArr(int * p){
	
	//可以通过 指针偏移 解引用的方式来取元素
	for(int i=0;i<5;i++){
		printf("数组的第 %d 位是： %d\n" , i , *(p+i));
	}

	printf("\n");
	
	// 也可以通过 [下标]的方式来取元素
	for(int i=0;i<5;i++){
		printf("数组的第 %d 位是： %d\n" , i , p[i]);
	}
}

int main(){
	
	int arr[5] = {10,20,30,40,50};
	
	//调用函数打印数组
	printArr(arr);
	
	
	
	return 0 ;
}
