cmake_minimum_required(VERSION 3.10)
project(SystemC2VHDL)
set(CMAKE_CXX_STANDARD 17)

add_executable(systemc2vhdl
    main.cpp
    parser.cpp
    vhdl_gen.cpp)
