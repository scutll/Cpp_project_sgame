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
   │--common.h       //游戏里会用到的一些变量/常量
   │--language.cpp
   │--language.h
   │--main.cpp       
   │--scene.h        
   │--scene.cpp      //方盘内容，使用_map来存储6*6数字
   │--main.cpp      //服务器运行或游玩开始程序
   │--player.cpp    //用户联机用
   └--utility.inl   //一些数学函数等小工具
```

### 构建方式
```bash
   build.bat
```

or 


'''cmd
md build
cd ./build
cmake ..
cmake --build .
'''