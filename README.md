# Cpp_project_sgame
C++大作业，活字飞花
## 项目结构
```bash
│--.gitignore  
│--build.bat        // Windows 一键编译脚本  
│--build.sh         // Linux/macOS 一键编译脚本  
│--CMakeLists.txt   // CMake 项目文件  
│--README.md     
└--src  
   │--block.cpp    //辅助存储行、列、十字格块  
   │--block.h        
   │--command.cpp    //命令类，绑定scece对象来实现操作方盘
   │--command.h     
   │--common.h       //游戏里会用到的一些变量/常量
   │--language.cpp
   │--language.h
   │--input.cpp      //接收输入
   │--input.h   
   │--main.cpp       
   │--scene.h        
   │--scene.cpp      //方盘内容，使用_map来存储6*6数字
   │--test.cpp       
   │--test.h  
   │--main.cpp      //服务器运行或游玩开始程序
   │--player.cpp    //用户联机用
   └--utility.inl   //一些数学函数等小工具
```
