cmake_minimum_required(VERSION 3.3)
project(cdr_parser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} `mysql_config --libs`")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/usr/lib64/mysql -lmysqlclient -lglog -lexplain -lpthread -lz -lm -lssl -lcrypto -ldl")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} `mysql_config --cflags`")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/mysql")


set(SOURCE_FILES
    BoyerHorspool.cpp
    BoyerHorspool.h
    CDRTokenizer.cpp
    CDRTokenizer.h
    definitions.h
    FileLoader.cpp
    FileLoader.h
    ini.c
    ini.h
    INIReader.cpp
    INIReader.h
    main.cpp
    MySQLData.cpp
    MySQLData.h
    MySQLHubs.cpp
    MySQLHubs.h
    MySQLOperStats.cpp
    MySQLOperStats.h
    MySQLOperators.cpp
    MySQLOperators.h)

add_executable(cdr_parser ${SOURCE_FILES})