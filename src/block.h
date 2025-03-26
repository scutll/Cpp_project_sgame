#ifndef __BLOCK_H_
#define __BOOCK_H_ 

#include "common.h"

class Block
{
protected:
    static const int MAX_COUNT = 6;
    int count;
    point_value_ *numbers[MAX_COUNT];

public:
    Block():count(0){}
    int getNum(int index);
    bool isFull() const;
    virtual bool isValid() const { return false; }
    // virtual void print() const;
    void Block::setNum(int index, int num);
    void push_back(point_value_ *point);
};

class Block_Row:public Block{
public:
    Block_Row() { Block(); }
    virtual bool isValid() const;
    virtual void print() const;
};

class Block_Column: public Block{
public:
    Block_Column() { Block(); }
    virtual bool isValid() const;
};


#endif