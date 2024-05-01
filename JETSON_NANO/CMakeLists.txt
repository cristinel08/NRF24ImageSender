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

find_package(OpenCV REQUIRED)

message(STATUS "ljetgpio_LIBS = ${GPIO_LIBS}")
message(STATUS "OpenCV_LIBS=${OpenCV_LIBS}")

file(GLOB EXECUTABLE
	src/*.cpp
	inc/*.h
)

set(INCLUDES
   {ljetgpio_INCLUDE_DIRS}
   {OpenCV_INCLUDE_DIRS}
   ${ROOT_DIR}/inc/
)

add_executable(${PROJECT_NAME} ${EXECUTABLE})

#target_compile_definitions(${PROJECT_NAME} PUBLIC RF24_NO_INTERRUPT)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

target_link_libraries(${PROJECT_NAME} "${GPIO_LIBS}")

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})
