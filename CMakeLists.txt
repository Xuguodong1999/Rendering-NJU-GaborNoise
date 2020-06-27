cmake_minimum_required(VERSION 3.15)
project(GaborNoise)
set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    # windows下修改到本地 Qt 库的cmake文件夹位置
    set(Qt5_DIR "C:/Qt/5.12.6/msvc2019_64/lib/cmake/Qt5")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
    # linux下修改到本地 Qt 库的cmake文件夹位置
    set(Qt5_DIR "/home/xgd/Qt/5.14.2/gcc_64/lib/cmake/Qt5")
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()
aux_source_directory(src DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)