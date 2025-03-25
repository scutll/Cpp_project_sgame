#ifndef COMMON_H_
#define COMMON_H_

#include <cassert>
//初始化时判断数字是否已经全部填上到格子里
enum class Status
{
    unfilled = 0, //还没填上数字
    filled  //已经填上数字
};

//表示点坐标
using point_ = struct point_{
    int x;
    int y;

    point_() : x(0), y(0) {}

    point_(int x,int y){
        this->x = x;
        this->y = y;
    }
};

//某坐标存储该结构，存放值
using point_value_ = struct point_value_{
    int value;
    Status status;

    point_value_(){
        this->value = 0;
        this->status = Status::unfilled;
    }

    point_value_(int num){
        assert(num <= 36 && num >= 1);

        this->status = Status::filled;
        this->value = num;
    }

    bool operator==(point_value_& oth){
        return oth.status == this->status&&oth.value == this->value;
    }
};

//用于比较两个坐标是否相同的函数对象
class CmpPoint{
public:
    bool operator()(const point_ &lhs,const point_& rhs){
        return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }
};

//键盘映射
enum class KeyMode : int
{
    NORMAL,
    
};

struct KeyMap
{
    //wsad
    // char UP = 0x77; 
    // char LEFT = 0x61;
    // char DOWN = 0x73;
    // char RIGHT = 0x64;
    char UP;
    char LEFT;
    char DOWN;
    char RIGHT;
    const char SWITCH = 0x6A; // 切换光标 j
    const char EXECUTE = 0x20; // 执行行列变换操作 space
    const char FINISH_CHECK = 0x0D; // 完成游戏判断是否成功 enter
    const char UNDO = 0x75;  //u
    const char ESC = 0x1B; //esc
};

struct NORMAL:KeyMap{
    NORMAL(){
    UP = 0x77; 
    LEFT = 0x61;
    DOWN = 0x73;
    RIGHT = 0x64;
    }
};

#endif