cmake_minimum_required(VERSION 3.13)
include(FetchContent)
# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)


# FetchContent_Declare(
#         emio
#         GIT_TAG main
#         GIT_REPOSITORY https://github.com/Viatorus/emio.git
#         GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(emio)


project(opendas_firm)

# initialize the Raspberry Pi Pico SDK

pico_sdk_init()
# set(HEADER_FILES ShiftRegister595.hpp)

add_executable(opendas_firm
    main.cpp
    ShiftRegister595.cpp
    VoltMeterInputs.cpp
    AFE.cpp
    ADS127L11/ADS127L11.cpp

    # ${HEADER_FILES}
)
pico_enable_stdio_usb(opendas_firm 1)
pico_enable_stdio_uart(opendas_firm 0)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(opendas_firm pico_stdlib hardware_spi)



# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(opendas_firm)


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