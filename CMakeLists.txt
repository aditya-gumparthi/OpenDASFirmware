cmake_minimum_required(VERSION 3.13)
include(FetchContent)
# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

set(SRC_DIRECTORY "${CMAKE_SOURCE_DIR}/src")
set(FREERTOS_SRC_DIRECTORY "${SRC_DIRECTORY}/FreeRTOS/FreeRTOS/Source")


# FetchContent_Declare(
#         emio
#         GIT_TAG main
#         GIT_REPOSITORY https://github.com/Viatorus/emio.git
#         GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(emio)


project(opendas_firmware)

# initialize the Raspberry Pi Pico SDK

pico_sdk_init()
# set(HEADER_FILES ShiftRegister595.hpp)


# Add FreeRTOS as a library
add_library(FreeRTOS STATIC
    ${FREERTOS_SRC_DIRECTORY}/event_groups.c
    ${FREERTOS_SRC_DIRECTORY}/list.c
    ${FREERTOS_SRC_DIRECTORY}/queue.c
    ${FREERTOS_SRC_DIRECTORY}/stream_buffer.c
    ${FREERTOS_SRC_DIRECTORY}/tasks.c
    ${FREERTOS_SRC_DIRECTORY}/timers.c
    #${FREERTOS_SRC_DIRECTORY}/portable/MemMang/heap_3.c
    ${FREERTOS_SRC_DIRECTORY}/portable/GCC/ARM_CM0/port.c
)


# Build FreeRTOS
target_include_directories(FreeRTOS PUBLIC
    ${SRC_DIRECTORY}/FreeRTOS_Config/
    ${FREERTOS_SRC_DIRECTORY}/include
    ${FREERTOS_SRC_DIRECTORY}/portable/GCC/ARM_CM0
)



add_executable(opendas_firmware
    ${SRC_DIRECTORY}/main.cpp
    ${SRC_DIRECTORY}/Cli.cpp
    ${SRC_DIRECTORY}/ShiftRegister595.cpp
    ${SRC_DIRECTORY}/VoltMeterInputs.cpp
    ${SRC_DIRECTORY}/AFE.cpp
    ${SRC_DIRECTORY}/ADS127L11/ADS127L11.cpp
    
)
pico_enable_stdio_usb(opendas_firmware 1)
pico_enable_stdio_uart(opendas_firmware 0)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(opendas_firmware pico_stdlib hardware_spi FreeRTOS)



# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(opendas_firmware)


# add_custom_command(TARGET ${PROJECT_NAME}
#                    POST_BUILD
#                    COMMAND readelf -a ${CMAKE_CURRENT_SOURCE_DIR}/build/${PROJECT_NAME}.elf
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#                    COMMENT "Get ELF Size"
#                    VERBATIM
# )   

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND SIZE -t ${CMAKE_CURRENT_SOURCE_DIR}/build/${PROJECT_NAME}.elf
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMENT "Get ELF Size"
                   VERBATIM
)   

string(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")