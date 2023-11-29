cmake_minimum_required(VERSION 3.20)

project(LineDetector LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#For Linux can be a problem with finding Qt, if it occurs, try $ sudo apt install libgl1-mesa-dev
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(SOURCES
    imagegenerator.cpp
    imageprocessor.cpp
    main.cpp
    angledeterminer1.cpp
    )
set(HEADERS
    imagegenerator.h
    imageprocessor.h
    mainwindow.cpp
    mainwindow.h
    angledeterminer1.h
    )

add_executable(line-detector ${SOURCES} ${HEADERS})

target_sources(line-detector PRIVATE mainwindow.ui)
target_include_directories(line-detector PUBLIC /usr/local/include/opencv4)
target_link_libraries(line-detector PUBLIC
        libopencv_core.so
        libopencv_ml.so
        libopencv_imgproc.so
        libopencv_features2d.so
        libopencv_photo.so
        libopencv_objdetect.so
        libopencv_video.so
        libopencv_highgui.so
        libopencv_gapi.so
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
	)
