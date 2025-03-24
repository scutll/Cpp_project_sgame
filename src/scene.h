#ifndef SCENE_H_
#define SCENE_H_

#include <iostream>
#include <vector>
#include "common.h"
#include "block.h"

class Scene{
public:
    Scene();
    ops Dete_OPMode() const;// 判断当前两个标记点所代表的操作
    //交换相邻两行/列
    void Switch_Column();
    void Switch_Row();
    void Switch_Cross(); //只有在同一行或同一列且两个标记点相隔两个格子才能交换十字
    
    void generate(); //生成图形

    void show() const; //打印图形

    bool isComplete() const;
    void play();
    bool save(const char *filename);
    bool load(const char *filename);

    void SetMode(KeyMode keymode);

private:
    void printUnderline(int line_ = -1) const;

private:
    //要调用类构造函数，就得将构造函数设置为public
    Block_Row _row_block[6];
    Block_Column _col_block[6];
    point_value_ map[36];
    point_ cur_point_1;
    point_ cur_point_2;
    int max_col;
    KeyMap *Keymap{};
    
    

    //创建Command类后创建vector命令历史
};

//三种操作
enum class ops : int
{
    SWITCH_COL,
    SWITCH_ROW,
    SWITCH_CROSS,
    MAX
};

#endif