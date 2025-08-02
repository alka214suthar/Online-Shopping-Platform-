cmake_minimum_required(VERSION 3.10)
project(MYSQL_CPP_PROJECT)

set(CMAKE_CXX_STANDARD 11)

# Path to MySQL Connector
include_directories("C:/Program Files/MySQL/MySQL Server 8.0/include")
link_directories("C:/Program Files/MySQL/MySQL Server 8.0/lib")

add_executable(main main.cpp)

target_link_libraries(main libmysql.lib)
