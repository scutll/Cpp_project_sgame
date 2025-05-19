#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "../../src/TerminalVersion/common.h"
#include "../../src/TerminalVersion/block.h"
#include "mainwindow.h"
#include "../../src/TerminalVersion/save_package.h"
//    0  1  2  3  4  5
//   ------------------>x
// 0 │0  1  2  3  4  5
// 1 │6  7  8  9  10 11
// 2 │12 13 14 15 16 17        坐标x,y对应x+y*6
// 3 │18 19 20 21 22 23
// 4 │24 25 26 27 28 29
// 5 │30 31 32 33 34 35
//   v
//   y
class Scene{
public:
    Scene();
    ops Dete_OPMode() const;// 判断当前两个标记点所代表的操作
    //交换相邻两行/列
    void Switch_Column();
    void Switch_Row();
    void Switch_Cross(bool up_down = true); //只有在同一行或同一列且两个标记点相隔两个格子才能交换十字
    void swap_point(point_ p1, point_ p2);

    // void setPoint(QPushButton* btn);
    void setPoint(int x, int y); // 设置cur_point 的坐标
    void execute(); //判断并执行操作
    void Switch_point(); //切换当前要移动的光标
    void Move(direction dirt); //cur_point_ 向指定方向移动一格
    void Move(ops op);
    //problem: 只能存上一次的，原因是undo的时候把undo的操作也加入队列了
    bool undo();
    void undo_(ops op);

    void generate(); //生成图形

    void show() const; //打印图形

    point_* curPoint() const{ return cur_point_;}
    bool isComplete() const;
    void play();
    void save();
    void load();
    void load(const package pkg);
    package package_(const char *name = "match_success");
    point_value_ map[36];


    void SetMode(KeyMode keymode);

    void bind_Window(MainWindow* window);
    MainWindow* window()const;

private:
    void printUnderline(int line_ = -1) const;

private:
    //要调用类构造函数，就得将构造函数设置为public

    Block_Row _row_block[6];
    Block_Column _col_block[6];
    point_ cur_point_1;
    point_ cur_point_2;
    point_* cur_point_;  //目前在控制的标记
    int max_col;
    KeyMap *Keymap{};
    std::vector<ops> Command_History;


    //绑定的游戏窗口
    MainWindow* w;
    //创建Command类后创建vector命令历史
};


#endif
