#include "scene.h"
#include "common.h"
#include "utility.inl"
#include <vector>
#include <cassert>
#include <cmath>

const int max_count = 36;

Scene::Scene():max_col(6){
    cur_point_1 = point_(0,0);
    cur_point_2 = point_(0,0);
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
            _col_block[i].push_back(&map[i + j * max_col]); //每一列
            _row_block[i].push_back(&map[i * max_col + j]); //每一行
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

ops Scene::Dete_OPMode() const{
    point_ p1 = cur_point_1;
    point_ p2 = cur_point_2;

    //相邻行交换
    if(p1.x == p2.x && abs(p1.y - p2.y) == 1)
    {
        return ops::SWITCH_ROW;
    }
    //相邻列
    else if(p1.y == p2.y && abs(p1.x - p2.x) == 1 )
    {
        return ops::SWITCH_COL;
    }
    //上下十字
    else if(p1.x == p2.x && abs(p1.y - p2.y) == 2)
    {
        return ops::SWITCH_CROSS_UD;
    }
    //左右十字
    else if(p1.y == p2.y && abs(p1.x - p2.x) == 2)
    {
        return ops::SWITCH_CROSS_LR;
    }
    else{
        return ops::MAX;
    }
}

void Scene::execute(){
    //判断当前两个标记位置所代表的操作
    ops op = Dete_OPMode();

    switch(op){
        //交换两列
        case ops::SWITCH_COL:
            break;
        // 交换两行
        case ops::SWITCH_ROW:
            break;
        // 上下交换十字
        case ops::SWITCH_CROSS_UD:
            break;
        // 左右交换十字
        case ops::SWITCH_CROSS_LR:
            break;
    }
}

void Scene::Switch_Column(){
    int x1 = cur_point_1.x;
    int x2 = cur_point_2.x;

    assert(x1 * x2 >= 0);

    for (int i = 0; i < max_col;i++){
        int tmp = _col_block[x1].getNum(i);
        _col_block[x1].setNum(i,_col_block[x2].getNum(i));
        _col_block[x2].setNum(i, tmp);
    }
}

void Scene::Switch_Row(){
    int y1 = cur_point_1.y;
    int y2 = cur_point_2.y;

    assert(y1 * y2 >= 0);

    for (int i = 0; i < max_col;i++){
        int tmp = _row_block[y1].getNum(i);
        _row_block[y1].setNum(i, _row_block[y2].getNum(i));
        _row_block[y2].setNum(i, tmp);
    }
}