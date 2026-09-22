#include <iostream>

/*
 * 引用：
 * 1. 引用就是原有数据的别名，操作引用等同于操作原有数据
 * 2. 引用不是一个新的变量，所以不会有新的内存开辟，不会占用新的内存空间
 * 3. 引用声明之后必须立即初始化，否则会报错。
 * 4. 引用的类型和原有数据的类型必须一样。
 * 5. 不能创建数组的引用，因为数组是一堆数据的集合。
 */
using namespace std;

int main() {

    int age = 18;
    cout <<"age的地址：" << &age <<endl;

    //引用
    int & age02 = age;
    cout <<"age02的地址：" << &age02 <<endl;

    //int & age03;

    age02 = 19;
    cout << "age=" << age <<endl;


    int score [] {10,20,30};
    //int & ss= score;


    return 0;
}
