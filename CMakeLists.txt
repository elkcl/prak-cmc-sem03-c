cmake_minimum_required(VERSION 3.26)
project(prak_cmc_sem03_c C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "-Wall -Werror -Wno-pointer-sign -Werror=vla -lm -fsanitize=address,undefined -ftrapv")

add_executable(test test.c)
add_executable(mz01-1 mz01/mz01-1.c)
add_executable(mz01-2 mz01/mz01-2.c)
add_executable(mz01-3 mz01/mz01-3.c)
add_executable(mz01-4 mz01/mz01-4.c)
add_executable(mz01-5 mz01/mz01-5.c)
add_executable(mz02-1 mz02/mz02-1.c)
add_executable(mz03-1 mz03/mz03-1.c)
add_executable(mz03-2 mz03/mz03-2.c)
add_executable(mz03-3 mz03/mz03-3.c)
add_executable(mz03-4 mz03/mz03-4.c)
add_executable(mz03-5 mz03/mz03-5.c)

add_executable(nikita-up05-5 nikita-up05-5.c)
add_executable(ira-up04-4 ira-up04-4.c)