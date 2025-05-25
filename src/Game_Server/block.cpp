#include "../../src/block.h"
#include "../../src/common.h"
#include <cassert>

//判断是否有相同数字
bool Block_Row::isValid() const{
    assert(MAX_COUNT == count); //false 则返回错误

    for (int i = 1; i < MAX_COUNT;i++){
        if(numbers[i]->status == Status::unfilled)
            continue;

        if(numbers[i]->value - numbers[i-1]->value != 1)
            return false;
    }
    return true;
}

bool Block_Column::isValid() const{
    assert(MAX_COUNT == count);

    for (int i = 1; i < MAX_COUNT;i++){
        if(numbers[i]->status == Status::unfilled)
            continue;

        if(numbers[i]->value - numbers[i-1]->value != 6)
            return false;
    }
    return true;
}

bool Block::isFull() const{
    for (int i = 0; i < MAX_COUNT;i++){
        if(numbers[i]==nullptr || numbers[i]->status == Status::unfilled)
            return false;
    }
    return true;
}


//将带数字的点填入块
void Block::push_back(point_value_* point){
    assert(point != nullptr);
    numbers[count++] = point;
}

//打印行块的内容
void Block_Row::print() const{
    //未实现
}

int Block::getNum(int index){
    if(numbers[index] == nullptr)
        return 0;
    else
        return numbers[index]->value;
}

void Block::setNum(int index,int num){
    if(numbers[index] == nullptr)
    {
        point_value_ point(num);
        numbers[index] = &point;
    }
    else
        numbers[index]->value = num;
    numbers[index]->status = Status::filled;
}