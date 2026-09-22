#include <iostream>
#include <string>
#include "stu.h"

using namespace std;

/*
    实现类当中的成员函数
 */
//只是剩下声明而已，所以这些要放到头文件中去
/*class stu{
public:
    string name;
    int age;
    void read();
};*/

//这段代码要放到具体源文件去
/*
void stu::read(){
    cout <<age <<" 岁的 " << name << " 在看书~~" << endl;
}
*/

int main() {

    stu s;
    s.name = "张三";
    s.age = 18;

    s.read();


    return 0;
}
