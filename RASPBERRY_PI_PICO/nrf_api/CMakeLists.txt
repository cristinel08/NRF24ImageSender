cmake_minimum_required(VERSION 3.10)

set(RF24_ROOT ${CMAKE_CURRENT_LIST_DIR})
message("NRF24_ROOT: " ${RF24_ROOT})
set(RF24 RF24)
project(${RF24} C CXX)

file(GLOB EXECUTABLE
    src/*.cpp
    inc/*.h
)

set(INC 
    ${RF24_ROOT}/inc/
)

add_library(${PROJECT_NAME} STATIC ${EXECUTABLE})

target_link_libraries(${PROJECT_NAME} PRIVATE
                        pico_stdlib 
                        hardware_spi 
                    )           

target_include_directories(${PROJECT_NAME} PUBLIC ${INC})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
# create map/bin/hex file etc.
# pico_add_extra_outputs(${PROJECT_NAME})

# add url via pico_set_program_url
# example_auto_set_url(${PROJECT_NAME})