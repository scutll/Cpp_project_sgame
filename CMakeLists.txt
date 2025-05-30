CMAKE_MINIMUM_REQUIRED(VERSION 3.12) # cmake最低版本要求

PROJECT(FloralType_server)    # 工程名 FloralType
# PROJECT(FloralType_player)    # 工程名 FloralType

set(CMAKE_CXX_STANDARD 17) # C++17标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_BUILD_TYPE "Release")  # Debug模式 选项: Release Debug MinSizeRel RelWithDebInfo
# 针对不同模式设置编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-O0 -Wall -g2)
else(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-O3 -Wall)
endif()

# set(CMAKE_VERBOSE_MAKEFILE ON) # 开启调试 出现问题时开启

# enable_testing() # 打开测试

add_definitions(-DDEBUG) # 相当于代码中 #define DEBUG

# add_subdirectory(test) # 添加test子目录

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin") # 可执行文件输出目录

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include") # 头文件包含目录
INCLUDE_DIRECTORIES("E:/mingw64/include") # 头文件包含目录

# 这段代码可以区分操作系统
MESSAGE("Identifying the OS...")
if(WIN32)
  MESSAGE("This is Windows.")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/source-charset:utf-8")
    add_compile_options("/execution-charset:utf-8")
  else()
    add_compile_options("-finput-charset=utf-8")
    add_compile_options("-fexec-charset=utf-8")
  endif()
elseif(APPLE)
  MESSAGE("This is MacOS.")
elseif(UNIX)
  MESSAGE("This is Linux.")
endif()
# 这段代码可以区分操作系统

AUX_SOURCE_DIRECTORY(src DIR_SRCS) # 添加源代码文件夹, 自动扫描所有文件

add_executable(FloralType_server  
    ${DIR_SRCS}
    main/server.cpp
)

add_executable(FloralType_player  
    ${DIR_SRCS}
    main/player.cpp
)

add_executable(FloralType_main  
    ${DIR_SRCS}
    main/main.cpp
)
