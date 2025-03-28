#include "common.h"
#include <chrono>
#include <ctime>
#include <iomanip> // std::put_time
// 构建一个保存包类

/*
info
    player_name
    date      保存时间 
    map[36]
*/
struct package{
    char player_name[20];
    std::tm date;
    point_value_ map[36];
};

