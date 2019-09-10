cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project( tic-tac )

find_package( OpenCV REQUIRED )

add_executable( tic-tac main.cpp )
add_executable( webcam webcam.cpp )
add_executable( thresh thresh.cpp )

target_link_libraries( tic-tac ${OpenCV_LIBS} )
target_link_libraries( webcam ${OpenCV_LIBS} )
target_link_libraries( thresh ${OpenCV_LIBS} )

