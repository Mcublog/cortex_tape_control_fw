include(${PROJECT_SOURCE_DIR}/hwtest/pcb_ctc_v1/common.cmake)

function(ctc_pcb_v1_add_hw_test targetname)

    # message("build tareget: ${targetname}: src: ${CMAKE_CURRENT_SOURCE_DIR}")

    # Add sources
    file(GLOB_RECURSE HWTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

    # Build lib
    add_executable(${targetname}
        ${HWTEST_SOURCES}
        ${DEBUG_SOURCES}
        ${HAL_SOURCES}
        ${ARCH_SOURCES}
        ${BSP_SOURCES}
        ${APP_SOURCES}
        ${RTT_SOURCES}
        # ${DWT_SOURCES}
        ${RING_BUFFER_SOURCES}
        ${SOFTWARE_TIMER_SOURCES}
    )

    # Include paths
    target_include_directories(${targetname}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/hwtest/ctc_v1/common
        ${CTC_V1_HAL_INCLUDE_DIRS}
        ${RTT_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/libs
        ${PROJECT_SOURCE_DIR}/libs/debug
        ${RING_BUFFER_INCLUDES}
        ${SOFTWARE_TIMER_INCLUDES}
        ${PROJECT_SOURCE_DIR}/bsp/ctc_v1
        ${PROJECT_SOURCE_DIR}/bsp/ctc_v1/configs
        ${PROJECT_SOURCE_DIR}/targets/ctc_v1
    )

    # Project symbols
    target_compile_definitions(${targetname} PRIVATE ${HWTEST_CTC_V1_DEFINES})

    # Compiler options
    target_compile_options(${targetname} PRIVATE ${ARM_NONE_EABI_FLAGS})

    # Linker options
    target_link_options(${targetname} PRIVATE ${F072_LINKER_OPTION})

    # Convert output to hex and binary
    add_custom_command(TARGET ${targetname}
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex -I elf32-littlearm ${targetname} ${targetname}.hex
        COMMAND ${CMAKE_OBJCOPY} ARGS -O binary -I elf32-littlearm ${targetname} ${targetname}.bin
    )

    build_jlink_script(${targetname} STM32F072CB)
endfunction()