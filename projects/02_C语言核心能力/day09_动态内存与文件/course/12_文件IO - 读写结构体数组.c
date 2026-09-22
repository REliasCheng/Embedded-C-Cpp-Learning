#include <stdio.h>

typedef struct{
	char name[20];
	int age ;
} Student;


int main(){
	
	//1. 创建结构体数组
	Student stus[] = { {"张三", 18} , {"李四", 28} , {"王五", 38} , {"赵六", 48}  };
	
	//计算原始的数据长度
	int size = sizeof(stus) / sizeof(Student);
	
	//开辟同等大小的数组备着
	Student stus2[size];
	
	//2. 打开文件
	FILE * f = fopen("stus.bin" , "wb+");
	
	//3. 写入结构体 :: 每次搬运 sizeof(Student)  这么多字节的数据，总共要搬运 size 次！
	fwrite(stus , sizeof(Student) , size , f);

	
	//4.关闭文件
	fclose(f);
	
	//=========================================
	
	//5. 打开文件
	FILE * f2 = fopen("stus.bin" , "rb");
	
	//6. 读取结构体
	fread(stus2 , sizeof(Student) ,size , f2 );
	
	//遍历打印
	for(int i=0;i<size;i++){
		
		Student s = stus2[i];
		printf("name=%s , age=%d\n" ,s.name ,s.age);
		
	}
	
	
	//7. 关闭文件
	fclose(f2);
	
	return 0;
}
