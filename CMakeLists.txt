cmake_minimum_required(VERSION 3.17)
project(udpapp)

set(CMAKE_CXX_STANDARD 14)

add_executable(udpapp main.cpp targetver.h stdafx.cpp stdafx.h crc.cpp crc.h sender.cpp sender.h utils.cpp utils.h receiver.h receiver.cpp md5.h)