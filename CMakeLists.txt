cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
project(hikvision VERSION 0.1.0)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
#库文件输出目录
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
#可执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(install_dir ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PREFIX_PATH ${install_dir}/cmake/modules)

#搜索
find_package(bb 4.0 REQUIRED NO_MODULE)
find_library(HCNetSDK HCNetSDK ${CMAKE_BINARY_DIR}/lib NO_DEFAULT_PATH)
find_library(HCCore HCCore ${CMAKE_BINARY_DIR}/lib NO_DEFAULT_PATH)

#生成可执行文件
add_executable(${PROJECT_NAME}_run main.cpp)
include_directories(${bb_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME}_run PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME}_run -lpthread bb ${HCCore} ${HCNetSDK})
