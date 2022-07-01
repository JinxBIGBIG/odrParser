//
// Created by JinxBIGBIG on 2022/6/27.
//

#ifndef ODRPARSER_DATAOPERATION_H
#define ODRPARSER_DATAOPERATION_H

#include <vector>
#include <iostream>

class DataOperation {



public:
    DataOperation();

    //(5, 10)——01010
    std::vector<int> Int2Binary(int figure, int intNumber);

    template<typename T>   //or class T
    void Print(std::vector<T> &t){
        std::cout << "Size = " << t.size() << std::endl;
        for(int i = 0; i < t.size(); i++)
        {
            std::cout << t.at(i);
        }
        std::cout << std::endl;
    };
};


#endif //ODRPARSER_DATAOPERATION_H
