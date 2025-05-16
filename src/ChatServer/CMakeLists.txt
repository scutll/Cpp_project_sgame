cmake_minimum_required(VERSION 3.16)

project(ChatServer LANGUAGES CXX)
set(CMAKE_PREFIX_PATH "E:/Qt/6.8.3/mingw_64/lib/cmake")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt6 COMPONENTS Network)



add_executable(ChatServer
  main.cpp
        cmake-build-debug/src/ClientWork.cpp
        cmake-build-debug/include/ClientWork.h
        cmake-build-debug/src/ChatServer.cpp
        cmake-build-debug/include/ChatServer.h
        cmake-build-debug/src/TcpServer.cpp
        cmake-build-debug/include/TcpServer.h
)
target_link_libraries(ChatServer PRIVATE Qt6::Core)
target_link_libraries(ChatServer PRIVATE Qt6::Network)


include(GNUInstallDirs)
install(TARGETS ChatServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
