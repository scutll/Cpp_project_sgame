#include "scene.h"
#include "common.h"
#include "color.h"
#include "display_symbol.h"
#include "language.h"
#include "utility.inl"
#include <vector>
#include <iomanip>
#include <cassert>
#include <iostream>
#include <cmath>

const int max_count = 36;

Scene::Scene():max_col(6){
    cur_point_1 = point_(0,0);
    cur_point_2 = point_(1,0);
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
    else if(p1.x == p2.x && abs(p1.y - p2.y) == 3)
    {
        return ops::SWITCH_CROSS_UD;
    }
    //左右十字
    else if(p1.y == p2.y && abs(p1.x - p2.x) == 3)
    {
        return ops::SWITCH_CROSS_LR;
    }
    else{
        return ops::MAX;
    }
}

void Scene::undo_(ops op){
    switch(op){
        //交换两列
        case ops::SWITCH_COL:
            Switch_Column();
            break;
        // 交换两行
        case ops::SWITCH_ROW:
            Switch_Row();
            break;
        // 上下交换十字
        case ops::SWITCH_CROSS_UD:
            Switch_Cross(true);
            break;
        // 左右交换十字
        case ops::SWITCH_CROSS_LR:
            Switch_Cross(false);
            break;
        case ops::SWITCH_POINT:
            Switch_point();
        default:
            break;
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
            break;
        // 交换两行
        case ops::SWITCH_ROW:
            Switch_Row();
            break;
        // 上下交换十字
        case ops::SWITCH_CROSS_UD:
            Switch_Cross(true);
            break;
            // 左右交换十字
        case ops::SWITCH_CROSS_LR:
            Switch_Cross(false);
        default:
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

void Scene::Switch_Cross(bool up_down){
    Cross cross1 = Cross(cur_point_1.x,cur_point_1.y);
    Cross cross2 = Cross(cur_point_2.x,cur_point_2.y);

    cross1.Form_Cross();
    cross2.Form_Cross();

    Cross* small_ = cross1 > cross2 ? &cross2 : &cross1;
    Cross* big_ = cross1 > cross2 ? &cross1 : &cross2;


    switch(up_down){
        case true:
            if(small_->up != nullptr)
                swap_point(*(small_->up), *(big_->down));
            if(small_->down != nullptr)
                swap_point(*(small_->down), *(big_->up));
            if(small_->left != nullptr)
                swap_point(*(small_->left), *(big_->left));
            if(small_->right != nullptr)
                swap_point(*(small_->right), *(big_->right));
            swap_point((small_->center_point), (big_->center_point));
            Command_History.push_back(ops::SWITCH_CROSS_UD);
            break;
        case false:
            if(small_->up != nullptr)
                swap_point(*(small_->up), *(big_->up));
            if(small_->down != nullptr)
                swap_point(*(small_->down), *(big_->down));
            if(small_->left != nullptr)
                swap_point(*(small_->left), *(big_->right));
            if(small_->right != nullptr)
                swap_point(*(small_->right), *(big_->left));
            Command_History.push_back(ops::SWITCH_CROSS_LR);
        default:
            swap_point((small_->center_point), (big_->center_point));
    }
}

void Scene::swap_point(point_ p1,point_ p2){
    int tmp = map[p1.x + p1.y * 6].value;
    map[p1.x + p1.y * 6].value = map[p2.x + p2.y*6].value;
    map[p2.x + p2.y * 6].value = tmp;
}

void Scene::setPoint(int x,int y){
    assert((x >= 0 && x <= 5) && (y >= 0 && y <= 5));

    cur_point_->x = x;
    cur_point_->y = y;
}


void Scene::Move(direction dirt){
    std::cout << "move" << std::endl;
    switch(dirt){
        case direction::UP:
            if(cur_point_->y > 0){
                cur_point_->y--;
                Command_History.push_back(ops::MOVE_UP);
                }
            break;
        case direction::DOWN:
            if(cur_point_->y < 5){
                cur_point_->y++;
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
            if(cur_point_->x < 5){
                Command_History.push_back(ops::MOVE_RIGHT);
                cur_point_->x++;
            }
    }

    assert((cur_point_->x >= 0 && cur_point_->x <= 5) || (cur_point_->y >= 0 || cur_point_->y <= 5));
}


void Scene::Move(ops op){
    assert(static_cast<int>(op & (ops::MOVE_DOWN | ops::MOVE_LEFT | ops::MOVE_RIGHT | ops::MOVE_UP)) != 0);

    direction dir;
    switch (op)
    {
    case ops::MOVE_DOWN:
        dir = direction::DOWN;
        break;
    case ops::MOVE_UP:
        dir = direction::UP;
        break;
    case ops::MOVE_LEFT:
        dir = direction::LEFT;
        break;
    case ops::MOVE_RIGHT:
        dir = direction::RIGHT;
    default:
        break;
    }

    Move(dir);
}

void Scene::show() const{

    //当前在操作的光标要打印成红色
    cls();
    // printUnderline();
    for (int j = 0; j < max_col; j++)
        std::cout << "     ";
    std::cout << std::endl;

    for (int i = 0; i < max_col; i++)
    {
        for (int j = 0; j < max_col; j++)
        {
            std::cout << "  " <<std::setw(2)<< map[i * 6 + j].value << " ";
        }
        std::cout << std::endl;
        
        // printUnderline();

        for (int j = 0; j < max_col; j++)
        {
            if ((!(cur_point_1.y == i && cur_point_1.x == j)) && (!(cur_point_2.y == i && cur_point_2.x == j)))
                std::cout << "     ";
            else if (cur_point_1.y == i && cur_point_1.x == j){
                if(cur_point_ == &cur_point_1)
                    std::cout <<Color::Modifier(Color::RESET,Color::BG_BLUE,Color::FG_BLUE)<< "  " << ARROW1 << "  "<<Color::Modifier();
                else
                    std::cout << "  " << ARROW2 << "  ";
            }
            else if (cur_point_2.y == i && cur_point_2.x == j){
                if(cur_point_ == &cur_point_2)
                    std::cout <<Color::Modifier(Color::RESET,Color::BG_BLUE,Color::FG_BLUE)<< "  "  << ARROW2 << "  "<<Color::Modifier();
                else
                    std::cout << "  " << ARROW2 << "  ";
            }
        }
        std::cout << std::endl;

    }
    std::cout << "p1:" << "( " << cur_point_1.x << " , " << cur_point_1.y << " )" << std::endl;
    std::cout << "p2:" << "( " << cur_point_2.x << " , " << cur_point_2.y << " )" << std::endl;
}


void Scene::SetMode(KeyMode mode){
    switch(mode){
        case  KeyMode::NORMAL:
            std::cout << "NORMAL" << std::endl;
            Keymap = new NORMAL;
            break;
        default:
            break;
    }
}


bool Scene::undo(){
    if(!Command_History.size()){
        send_msg(I18n::Instance().getKey(I18n::Key::UNDOERROR));
        return false;
    }
    ops undo = Command_History.back();
    Command_History.pop_back();

    if (undo == ops::MOVE_DOWN)
        undo = ops::MOVE_UP;
    else if (undo == ops::MOVE_UP)
        undo = ops::MOVE_DOWN;
    else if (undo == ops::MOVE_LEFT)
        undo = ops::MOVE_RIGHT;
    else if (undo == ops::MOVE_RIGHT)
        undo = ops::MOVE_LEFT;
    
    if(static_cast<int>(undo & (ops::MOVE_DOWN|ops::MOVE_LEFT|ops::MOVE_RIGHT|ops::MOVE_UP)) != 0)
        Move(undo);
    else if(static_cast<int>(undo &(ops::SWITCH_ROW|ops::SWITCH_COL|ops::SWITCH_CROSS_LR|ops::SWITCH_CROSS_UD|ops::SWITCH_POINT)) != 0)
        undo_(undo);
    else{
            send_msg(I18n::Instance().getKey(I18n::Key::UNDOERROR));
            std::cout << "fail to undo!";
            return false;
    }

    return true;
}


void Scene::play(){
    show();

    char key = '\0';
    
    while(true){
        key = _getch();
        std::cout << key << std::endl;


        if(key == Keymap->UP){
            Move(direction::UP);
            show();
        }
        else if(key == Keymap->DOWN){
            Move(direction::DOWN);
            show();
        }
        else if(key == Keymap->LEFT){
            Move(direction::LEFT);
            show();
        }       
        else if(key == Keymap->RIGHT){
            std::cout << "right" << std::endl;
            Move(direction::RIGHT);
            show();
        }
        else if(key == Keymap->SWITCH){
            Switch_point();
            show();
        }
        else if(key == Keymap->EXECUTE){
            execute();
            show();
        }
        else if(key == Keymap->UNDO){
            if(undo())
                show();
        }
        else if(key == Keymap->FINISH_CHECK){
            //检查是否完成 完成则祝贺任意键退出否则继续
            if(isComplete()){
                send_msg(I18n::Instance().getKey(I18n::Key::CONGRATULATION));
                getchar();
                exit(0);
            }
            else{
                send_msg(I18n::Instance().getKey(I18n::Key::NOT_COMPLETED));
            }
        }
        else if(key == Keymap->ESC){
            send_msg(I18n::Instance().getKey(I18n::Key::ASK_QUIT));
            char q = getchar();
            if(q == 'y' || q == 'Y'){
                cls();
                exit(0);
            }
        }
        else{
            send_msg(I18n::Instance().getKey(I18n::Key::INPUT_ERROR));
        }
    }
}