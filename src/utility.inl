#ifndef UTILITY_INL_
#define UTILITY_INL_

#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
//获取随机数
inline unsigned int random(int begin, int end)
{
    std::random_device rd;  //随机种子
    std::mt19937 g(rd());   //随机数生成器
    return std::uniform_int_distribution<unsigned int>(begin, end)(g);
    //生成均匀分布的随机整数
}

//获取含1-36的数组
inline std::vector<int> get_unit(int count=36){
    std::vector<int> unit(count);
    for (int i = 0; i < unit.size();i++){
        unit[i] = i + 1;
    }
    return unit;
}   


//将数组顺序打乱
inline void Shuffle(std::vector<int>& unit){
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(unit.begin(), unit.end(), g);
}

inline void send_msg(const char* msg,bool lf = true){
    std::cout << msg;
    if(lf)
        std::cout << std::endl;
}

inline void send_msg(const std::string& msg,bool lf = true){
    send_msg(msg.c_str(), lf);
}


//windows系统引入接收键盘信息所用的头文件
#ifdef _WIN32
#include <conio.h>
#endif

//清屏
inline void cls(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

#endif