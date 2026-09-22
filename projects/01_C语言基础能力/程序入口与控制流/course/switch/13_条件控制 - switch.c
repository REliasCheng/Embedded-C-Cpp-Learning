#include<stdio.h>
#include<stdint.h>
int main(){
	
	//请输入分数等级【A ~ D】
	
	printf("请输入考试分数评级：\n");
	
	char level ;
	scanf("%c" , &level);
	
	//() 就放要判断的变量 :: 每一个case的后面必须有break ，否则代码会继续往下执行，直到遇见break.
	switch(level){
		
	case 'A':
		printf("优秀~！");
		break;
		
	case 'B':
		printf("良好~！");
		break;
		
	case 'C':
		printf("及格~！");
		break;
		
	case 'D':
		printf("较差~！");
		break;
		
	default: //如果前面都不匹配，就进入这个分支
		printf("输入错误~！");
		break;
		
	}
	
	
	return 0;
}
