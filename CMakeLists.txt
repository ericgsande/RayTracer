cmake_minimum_required(VERSION 2.6)

project(rayCasting C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(MY_SOURCES
	main.cpp
	Vector3.h
	Vector3.cpp
	Sphere.h
	Sphere.cpp
	Ray.h
	Ray.cpp
	Color.h
	Color.cpp
	Face.h
	Face.cpp
)

add_executable(main ${MY_SOURCES})
