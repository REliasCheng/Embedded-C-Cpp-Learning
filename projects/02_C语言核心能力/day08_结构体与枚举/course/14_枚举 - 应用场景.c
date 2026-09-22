#include <stdio.h>

enum Gender{
	MALE , FEMALE
};

struct  Student{
	char name[20];
	int age ;
	//char gender[10];  // 性别，我们不想让别人随意输入，因为只要是字符串，它就可以随便输入。
	enum Gender gender;
};


// 定义一个学生结构体，其中要包含有性别的属性，并且性别必须是枚举类型。
// 在main函数当中定义学生的数组，存储若干个学生，然后判定，男生有几个，女生有几个！


int main(){
	//1. 创建学生结构体变量
	//[错误] incompatible types when initializing type 'enum Gender' using type 'char *'
	//struct Student s = {"张三" ,18 , "哈哈"};
	
	struct Student s = {"张三" ,18 , FEMALE};
	
	printf("姓名: %s \n", s.name);
	printf("年龄: %d \n", s.age);
	printf("性别: %s \n", s.gender == MALE ? "男" : "女");
}


