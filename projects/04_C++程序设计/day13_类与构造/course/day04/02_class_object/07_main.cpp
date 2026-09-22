#include <iostream>
#include <string>

using namespace std;

/*
    构造函数
        调用时机： 创建对象的时候调用
        作用 ： 用来初始化数据
 */

class stu{
public:

    string name;
    int age;

    //构造函数
    stu(){
        cout <<"无参构造函数~" << endl;
        name = "张三";
        age = 18;
    }

    stu(string n , int a){
        cout << "有参构造函数" <<endl;
        name = n;
        age = a;
    }
    /*stu (int a){
        cout << "有参构造函数" <<endl;
    }*/
};

int main() {

    stu s1;
    /*s1.name = "张三";
    s1.age = 18 ;*/


    stu s2("李四" ,19); // ctrl +  p
    cout << s2.name << "=" << s2.age <<endl;

    return 0;
}
