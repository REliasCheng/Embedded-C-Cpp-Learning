#include <stdio.h>
#include "calc.h"

#include "A.h"
#include "B.h"

/*
  导入头文件的两种写法：
	  1. 导入头文件其实代表包含关系，也就是把头文件的所有内容，全部放到这个#include 的位置来。
	  2. <>：属于导入编译器 MINGW 提供的头文件.. 找这个头文件，会去MINGW文件夹里面找
	  3. "": 
		  3.1 属于导入我们在项目里面写的头文件.. 找这个头文件，会去项目的根目录里面找
		  3.2 如果在项目的根目录里面找不到，编译器还会去MINGW文件夹里面找
 */

/*
  1. 新建项目..
  2. 编写main.c的代码
  3. 创建calc.h 和 calc.c文件
  4. 把函数的声明搬运到calc.h 里面去 ，把函数的定义搬运到calc.c 里面去
  5. 在main.c的顶部导入 calc.h  在calc.c的顶部导入calc.h
 */

int main(){
	
	sayHi();
	
	return 0;
}

