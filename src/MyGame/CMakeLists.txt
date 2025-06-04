cmake_minimum_required(VERSION 3.16)
set(QT_MAJOR_VERSION 6)
set(CMAKE_PREFIX_PATH "E:/Qt/6.8.3/mingw_64/lib/cmake")
project(MyGame VERSION 0.1 LANGUAGES CXX)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 COMPONENTS Network)

# # 添加网络模块，下面还有一个target_link_libraries
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MyGame
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mainwindow.ui
        scene.cpp
        block.cpp
        language.cpp
        scene.h
        start_window.h start_window.cpp start_window.ui
        archives.h archives.cpp archives.ui
        player_connect.cpp
        player_connect.h
        player_connect.cpp
        Client.cpp Client.h ClientNetworkManager.cpp ClientNetworkManager.h global.cpp global.h
        asklogin.h asklogin.cpp asklogin.ui
        askregister.h askregister.cpp askregister.ui
        playernetwork.h playernetwork.cpp
        playerclient.h playerclient.cpp


    )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET MyGame APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MyGame SHARED
            ${PROJECT_SOURCES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MyGame
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MyGame PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(MyGame PRIVATE Qt6::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MyGame)
endif()

set_target_properties(MyGame PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MyGame
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MyGame)
endif()
