cmake_minimum_required(VERSION 3.24)
project(du_command)

set(CMAKE_CXX_STANDARD 17)

add_executable(du
        main.cpp
        ArgumentParser.cpp
        ArgumentParser.h
        PathSizeCounter.cpp
        SizeCounter.h)
