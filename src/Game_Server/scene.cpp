#include "../../src/scene.h"
#include "../../src/save_package.h"
#include <random>
#include <algorithm>
#include <chrono>
#include <ctime>
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


const int max_count = 36;
Scene::Scene() : max_col(6)
{
    cur_point_1 = point_(0, 0);
    cur_point_2 = point_(1, 0);
    cur_point_ = &cur_point_1;
}


void Scene::generate()
{
    // 生成地图
    /*
    将打乱的数字逐个填入map
    按行、列填入block
    */

    std::vector<int> unit = get_unit(max_count);
    Shuffle(unit);

    for (int i = 0; i < max_count; i++)
    {
        map[i].value = unit[i];
        map[i].status = Status::filled;
    }

    for (int i = 0; i < max_col; i++)
    {
        for (int j = 0; j < max_col; j++)
        {
            _col_block[i].push_back(&map[i + j * max_col]); // 每一列
            _row_block[i].push_back(&map[i * max_col + j]); // 每一行
        }
    }
}

package Scene::package_(const char *name)
{
    package _package;
    memcpy(_package.archive_name, name, 20);

    // 当前时间
    auto now = std::chrono::system_clock::now();

    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_now = *std::localtime(&now_time);
    _package.date = local_now;

    // 载入地图
    for (int i = 0; i < max_col * max_col; i++)
        _package.map[i] = map[i];

    return _package;
}
