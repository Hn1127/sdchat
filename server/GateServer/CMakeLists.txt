cmake_minimum_required(VERSION 3.20.3)

project(GateServer)

set(CMAKE_CXX_STANDARD 11)

# 添加boost库
include_directories("D:/Develop/ThirdLib/boost_1_86_0")

# 添加jsonapp库
include_directories("D:/Develop/ThirdLib/jsoncpp/include")

link_directories("D:/Develop/ThirdLib/jsoncpp/build/lib")
link_libraries(jsoncpp.a)

link_libraries(ws2_32.lib)
link_libraries(wsock32.dll)

add_subdirectory(src)