cmake_minimum_required(VERSION 3.10)

set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

project(nrf24l01)
set (ljetgpio_INCLUDE_DIRS
	/usr/include/
)
#find_library(RF24 rf24 REQUIRED)
#find_package(JetsonGPIO)

#message(STATUS "using RF24 library: ${RF24}")

find_library(GPIO_LIBS
	     NAMES jetgpio
	     PATHS /usr/lib
	     REQUIRED
)
set(DARKNET  ~/00GIT/darknet/)
find_library(DARKNET_LIB
	     NAMES "darknet"
	     PATHS ~/00GIT/darknet/
	     REQUIRED
)
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
option(OPENCV ON)
message(STATUS "ljetgpio_LIBS = ${GPIO_LIBS}")
message(STATUS "DARKNET_LIBS = ${DARKNET_LIB}")
message(STATUS "OpenCV_LIBS=${OpenCV_LIBS}")

file(GLOB EXECUTABLE
	src/*.cpp
	inc/*.h
	#${ROOT_DIR}/src/main.cpp
	#${ljetgpio_INCLUDE_DIRS}/jetgpio.h
)

set(INCLUDES
   {ljetgpio_INCLUDE_DIRS}
   {OpenCV_INCLUDE_DIRS}
   ${ROOT_DIR}/inc/
   ${DARKNET}/include/
   ${ROOT_DIR}/3rdparty/stb/include/
)

add_executable(${PROJECT_NAME} ${EXECUTABLE})

#target_compile_definitions(${PROJECT_NAME} PUBLIC RF24_NO_INTERRUPT)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries(${PROJECT_NAME} "${GPIO_LIBS}")

target_compile_definitions(${PROJECT_NAME} PRIVATE  "OPENCV")

target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} "${DARKNET_LIB}")

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})
