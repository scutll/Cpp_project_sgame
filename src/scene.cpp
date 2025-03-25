#include "scene.h"
#include "common.h"
#include "display_symbol.h"
#include "utility.inl"
#include <vector>
#include <cassert>
#include <iostream>
#include <cmath>

const int max_count = 36;

Scene::Scene():max_col(6){
    cur_point_1 = point_(0,0);
    cur_point_2 = point_(0,0);
    cur_point_ = &cur_point_1;
}


void Scene::Switch_point(){
    if (cur_point_ == &cur_point_1)
        cur_point_ = &cur_point_2;
    else
        cur_point_ = &cur_point_1;
    Command_History.push_back(ops::SWITCH_POINT);
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
    //还要将操作历史存入数组
    ops op = Dete_OPMode();

    switch(op){
        //交换两列
        case ops::SWITCH_COL:
            Switch_Column();
        // 交换两行
        case ops::SWITCH_ROW:
            Switch_Row();
        // 上下交换十字
        case ops::SWITCH_CROSS_UD:
            Switch_Cross(true);
        // 左右交换十字
        case ops::SWITCH_CROSS_LR:
            Switch_Cross(false);
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

    Command_History.push_back(ops::SWITCH_COL);
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

    Command_History.push_back(ops::SWITCH_ROW);
}

void Scene::Switch_Cross(bool up_down = true){
    Cross cross1 = Cross(cur_point_1);
    Cross cross2 = Cross(cur_point_2);

    Cross small_ = cross1 > cross2 ? cross2 : cross1;
    Cross big_ = cross1 > cross2 ? cross1 : cross2;


    switch(up_down){
        case true:
            if(small_.up != nullptr)
                swap_point(*(small_.up), *(big_.down));
            if(small_.down != nullptr)
                swap_point(*(small_.down), *(big_.up));
            if(small_.left != nullptr)
                swap_point(*(small_.left), *(big_.left));
            if(small_.right != nullptr)
                swap_point(*(small_.right), *(big_.right));
            Command_History.push_back(ops::SWITCH_CROSS_UD);
            break;
        case false:
            if(small_.up != nullptr)
                swap_point(*(small_.up), *(big_.up));
            if(small_.down != nullptr)
                swap_point(*(small_.down), *(big_.down));
            if(small_.left != nullptr)
                swap_point(*(small_.left), *(big_.right));
            if(small_.right != nullptr)
                swap_point(*(small_.right), *(big_.left));
            Command_History.push_back(ops::SWITCH_CROSS_LR);
    }
}

void Scene::swap_point(point_ p1,point_ p2){
    int tmp = map[p1.x + p1.y * 6].value;
    map[p1.x + p1.y * 6].value = map[p2.x + p2.y].value;
    map[p2.x + p2.y * 6].value = tmp;
}


void Scene::Move(direction dirt){
    switch(dirt){
        case direction::UP:
            if(cur_point_->y > 0){
                cur_point_->y++;
                Command_History.push_back(ops::MOVE_UP);
                }
            break;
        case direction::DOWN:
            if(cur_point_->y < 5){
                cur_point_->y--;
                Command_History.push_back(ops::MOVE_DOWN);
            }
            break;
        case direction::LEFT:
            if(cur_point_->x > 0){
                cur_point_->x--;
                Command_History.push_back(ops::MOVE_LEFT);
            }
            break;
        case direction::RIGHT:
            if(cur_point_->x > 0){
                Command_History.push_back(ops::MOVE_RIGHT);
                cur_point_->x++;
            }
    }
}

void Scene::show() const{
    // printUnderline();
    for (int j = 0; j < max_col; j++)
        std::cout << "   ";
    std::cout << std::endl;

    for (int i = 0; i < max_col; i++)
    {
        for (int j = 0; j < max_col; j++)
        {
            std::cout << " " << map[i * 6 + j].value << " ";
        }
        std::cout << std::endl;
        // printUnderline();
        for (int j = 0; j < max_col; j++)
        {
            if ((!(cur_point_1.y == i && cur_point_1.x == j)) && (!(cur_point_2.y == i && cur_point_2.x == j)))
                continue;
            else if (cur_point_1.y == i && cur_point_1.x == j)
                std::cout << " " << ARROW1 << " ";
            else if (cur_point_2.y == i && cur_point_2.x == j)
                std::cout << " " << ARROW2 << " ";
        }
        std::cout << std::endl;
        }
}