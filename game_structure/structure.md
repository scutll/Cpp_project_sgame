
# common.h
- 定义一些要用到的变量、类型
- struct point 坐标的x,y值
- struct point_value 一个有值的点
- KeyMap 键盘映射(上下左右、撤回、退出、提交、更换指针)

# utility.inl
- 包含各种内联函数工具的文件
```C++
random() //生成范围内的随机整数
get_unit()  //返回一个包含1-36的整数向量
shuffle_unit() //将1-36的整数向量的顺序打乱
message() //向控制台打印msg
cls() //在windows和linux下分别调用cls或clear清屏
getch() //从控制台读取一个字符，无需回车键 (windows下#include <conio.h>)
```

# block.h
- 定义block类，用于管理方盘中一整行或一整列或是十字区域
```C++
MAX_COUNT = 9
CROSS_CORNER = 3
CROSS_SIDE = 4
CROSS_NORMAL = 5
_count 已存储的单元格数量
point_value _units_rc[MAX_COUNT] //行或列
struct _units_cross //十字按上下左右存储对应坐标并存储类型(优先级比较 角落>边>普通)
```
# scene.h scene.cpp
- 负责管理游戏的场景、状态和逻辑
使用类Scene管理游戏方盘
Attributes:
```C++
max_column = 6
max_row = 6

_map (point_value 数组) 所有单元格
_column_block (point_value* 数组) 按列存放所有_map里面的单元
_row_block (point_value* 数组) 按行存放所有_map里面的单元
_cur_point1 (point*) 当前游戏指针1所在单元格
_cur_point2 (point*) 当前游戏指针1所在单元格
_cross_map (_unit_cross[max_column*max_row]) //每个单元对应一个，方便快速找到上下左右的格子
std::vector<CCommand> CommandHistory; 存储命令历史，支持撤销
```


