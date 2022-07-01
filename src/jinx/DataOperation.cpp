//
// Created by JinxBIGBIG on 2022/6/27.
//

#include "jinx/DataOperation.h"

using namespace std;


DataOperation::DataOperation() {}


std::vector<int> DataOperation::Int2Binary(int figure, int intNumber) {
    vector<int> binaryResult;
    for(int i = figure - 1; i >= 0; i--)
    {
        binaryResult.push_back( ( (intNumber >> i) & 1) );//与1做位操作，前面位数均为0
        //cout<<( (n>>i) & 1);//输出二进制
    }
    //cout<<endl;
    return binaryResult;
}
/*
template<typename T>
void DataOperation::Print(vector<T> &t) {
    cout << "Size = " << t.size() << endl;
    for(int i = 0; i < t.size(); i++)
    {
        cout << t.at(i);
    }
    cout << endl;

}*/


/*int main(int argc, char** argv){
    DataOperation data;
    vector<int> a;
    a = data.Int2Binary(5, 10);
    data.Print(a);

}*/
