#include <stdio.h>
#include <string.h>

//结构体和函数  :::::   返回值返回结构体
struct student{
	char name[20];
	int age;
};

// =================== 返回结构体变量 =====================
struct student createStudent(){
	struct student s = {"张三", 18};
	return s;
}


// =================== 返回结构体指针 =====================
struct student * modifyStudent(struct student * s){

	s->age = 29;

	return s;
}


int main(){
	//创建学生
	struct student stu = createStudent();
	printf("111name:%s ,age:%d\n", stu.name ,stu.age);
	
	struct student * s = modifyStudent(&stu);
	printf("222name:%s ,age:%d\n", (*s).name , s->age);
}


