cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project( tic-tac )
find_package( OpenCV REQUIRED )
find_package( Lua REQUIRED )
include_directories( ${LUA_INCLUDE_DIR} )

add_executable( tic-tac main.cpp board.cpp thresh.cpp plotter.cpp)
# add_executable( thresh thresh.cpp )
add_executable( webcam utils/webcam.cpp )
#add_executable( luatest luatest.cpp )

target_link_libraries( tic-tac ${OpenCV_LIBS} ${LUA_LIBRARIES})
target_link_libraries( webcam ${OpenCV_LIBS} )
#target_link_libraries( thresh ${OpenCV_LIBS} )
#target_link_libraries( luatest ${LUA_LIBRARIES} )

