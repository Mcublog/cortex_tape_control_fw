include(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/mcu_f0.cmake)
include(${PROJECT_SOURCE_DIR}/libs/libs.cmake)
include(${PROJECT_SOURCE_DIR}/libs/hw/hw.cmake)
include(${PROJECT_SOURCE_DIR}/libs/hw/third_party/third_party.cmake)


function(fw_app_build_script targetname extra_option)
    # Add sources
    file(GLOB_RECURSE HAL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/*.s
    )

    file(GLOB_RECURSE APP_SOURCES
        ${PROJECT_SOURCE_DIR}/app/*.c
        ${PROJECT_SOURCE_DIR}/app/*.cpp)

    file(GLOB_RECURSE ARCH_SOURCES
        ${PROJECT_SOURCE_DIR}/arch/stm32f0/*.c
        ${PROJECT_SOURCE_DIR}/arch/stm32f0/*.cpp)

    file(GLOB_RECURSE BSP_SOURCES
        ${PROJECT_SOURCE_DIR}/bsp/ctc_v1/*.c
        ${PROJECT_SOURCE_DIR}/bsp/ctc_v1/*.cpp)


    # Build lib
    add_executable(${targetname}
        ${LOG_DEBUG_SOURCES}
        ${HAL_SOURCES}
        ${ARCH_SOURCES}
        ${BSP_SOURCES}
        ${APP_SOURCES}
        ${RTT_SOURCES}
        # ${DWT_SOURCES}
        ${RING_BUFFER_SOURCES}
        ${SOFTWARE_TIMER_SOURCES}
        ${GMG12864_SOURCES}
        ${COBS_SOURCES}
    )

    # Include paths
    target_include_directories(${targetname}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}
        ${QM_V1_HAL_INCLUDE_DIRS}
        ${APP_DIRS}
        ${RTT_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/libs
        ${PROJECT_SOURCE_DIR}/libs/debug
        ${RING_BUFFER_INCLUDES}
        ${SOFTWARE_TIMER_INCLUDES}
        ${PROJECT_SOURCE_DIR}/bsp/qm_v1/configs
        ${PROJECT_SOURCE_DIR}/bsp/qm_v1
        ${GMG12864_INCLUDES}
        ${RINGFS_INCLUDES}
        ${COBS_INCLUDES}
    )

    # Project symbols
    set(DEFINES
        ${QM_V1_DEFINES}
        ${extra_option}
    )
    target_compile_definitions(${targetname} PRIVATE ${DEFINES})

    # Compiler options
    target_compile_options(${targetname} PRIVATE ${ARM_NONE_EABI_FLAGS})

    # Linker options
    target_link_options(${targetname} PRIVATE ${F072_LINKER_OPTION})

    # # Convert output to hex and binary
    add_custom_command(TARGET ${targetname}
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex -I elf32-littlearm ${targetname} ${targetname}.hex
        COMMAND ${CMAKE_OBJCOPY} ARGS -O binary -I elf32-littlearm ${targetname} ${targetname}.bin
    )

    build_jlink_script(${targetname} STM32F072CB)
endfunction()

fw_app_build_script(fw_ctc_v1 "")
