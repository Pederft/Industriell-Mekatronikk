cmake_minimum_required(VERSION 3.8)
project(publisher_mqtt)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Mosquitto IMPORTED_TARGET libmosquitto REQUIRED)

add_executable(publisher publisher.c)
target_link_libraries(publisher PkgConfig::Mosquitto)