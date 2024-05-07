cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

project(ImageTransmission C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

file(GLOB EXECUTABLES
    main.cpp)

# Initialize the SDK
pico_sdk_init()

include(example_auto_set_url.cmake)
# Add blink example
#add_subdirectory(blink)


add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

add_subdirectory(camera_api)
add_subdirectory(nrf_api)

add_executable(${PROJECT_NAME} ${EXECUTABLES})

target_link_libraries(${PROJECT_NAME} PUBLIC
                      RF24
                      CameraOV2640API)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
# # create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})

# add url via pico_set_program_url
example_auto_set_url(${PROJECT_NAME})
# set(INC 
#     ${CAMERA_ROOT}/inc/
#     ${RF24_ROOT}/inc/
# )

# add_executable(${PROJECT_NAME} ${EXECUTABLES})




