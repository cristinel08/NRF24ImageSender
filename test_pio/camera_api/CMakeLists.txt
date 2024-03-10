cmake_minimum_required(VERSION 3.10)

set(CAMERA_ROOT ${CMAKE_CURRENT_LIST_DIR})
message("CAMERA_ROOT: " ${CAMERA_ROOT})
project(CameraOV2640API C CXX)

file(GLOB EXECUTABLE
    src/*.cpp
    inc/*.h
)

set(INC 
    ${CAMERA_ROOT}/inc/
)

add_executable(${PROJECT_NAME} ${EXECUTABLE})

target_link_libraries(${PROJECT_NAME} PUBLIC 
                        pico_stdlib 
                        hardware_i2c 
                        hardware_spi 
                        hardware_irq	
                        hardware_dma
                        hardware_pwm
                    )           

target_include_directories(${PROJECT_NAME} PUBLIC ${INC})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})

# add url via pico_set_program_url
example_auto_set_url(${PROJECT_NAME})