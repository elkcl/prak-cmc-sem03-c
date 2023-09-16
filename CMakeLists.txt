cmake_minimum_required(VERSION 3.26)
project(prak_cmc_sem03_c C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "-O2 -Wall -Werror -Wno-pointer-sign -Werror=vla -lm")

add_executable(test test.c)
add_executable(mz01-1 mz01/mz01-1.c)
add_executable(mz01-2 mz01/mz01-2.c)
add_executable(mz01-3 mz01/mz01-3.c)
add_executable(mz01-4 mz01/mz01-4.c)
add_executable(mz01-5 mz01/mz01-5.c)
add_executable(mz02-1 mz02/mz02-1.c)