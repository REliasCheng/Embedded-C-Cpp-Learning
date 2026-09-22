#include <iostream>
#include <vector> // 1. 导入头


//vector的增删改查

//使用命名空间std
using namespace std;

int main(){

    //1. 定义vector
    vector <int> scores{100,90,80,70,60,50};
    
    //2. 增加数据
    scores.push_back(66);
    scores.push_back(77);
    
    //3. 修改数据
    scores[0]=55;
    scores.at(1) = 56;
   
    //4. 查询数据
    cout << "scores[0]=" << scores.at(0) << endl;
    
    //5. 遍历vector
    for (int i = 0; i < scores.size(); ++i) {
        cout << i <<"===" << scores[i] <<endl;
    }

    //6. 删除数据。
    //想删除某一个数据，那么需要得到某一个数据对应的迭代器才能删除。

    //删除第0个元素
    scores.erase(scores.begin());

    cout << "----------------------------------" << endl;
    for (int i = 0; i < scores.size(); ++i) {
        cout << i <<"===" << scores[i] <<endl;
    }

    return 0 ;
}

