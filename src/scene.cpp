#include "scene.h"
#include "common.h"
#include "utility.inl"
#include <vector>

const int max_count = 36;

Scene::Scene():max_col(6){
    cur_point_1 = point_({0, 0});
    cur_point_2 = point_({0, 0});
}

void Scene::generate(){
    //生成地图
    /*
    将打乱的数字逐个填入map
    按行、列填入block
    */

    std::vector<int> unit = get_unit(max_count);
    Shuffle(unit);

    for (int i = 0; i < max_count;i++){
        map[i].value = unit[i];
        map[i].status = Status::filled;
    }

    for (int i = 0; i < max_col;i++){
        for (int j = 0; j < max_col;j++){
            _col_block[i].push_back(&map[i + j * max_col]);
            _row_block[i].push_back(&map[i * max_col + j]);
        }
    }




}

//先检查是否全部被填满，然后检查是否完成游戏
bool Scene::isComplete() const{

    for (int i = 0; i < max_col;i++)
        if(!_col_block[i].isFull())
            return false;

    for (int i = 0; i < max_col;i++){
        if((!_col_block[i].isValid())||(!_row_block[i].isValid()))
            return false;
    }

    return true;
}