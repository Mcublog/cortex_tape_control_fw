include(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/mcu_f0.cmake)
include(${PROJECT_SOURCE_DIR}/libs/libs.cmake)
include(${PROJECT_SOURCE_DIR}/libs/hw/hw.cmake)
include(${PROJECT_SOURCE_DIR}/libs/hw/third_party/third_party.cmake)

# Add sources
file(GLOB_RECURSE HAL_SOURCES
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/*.c
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/*.s
)

file(GLOB_RECURSE APP_SOURCES
    ${PROJECT_SOURCE_DIR}/app/*.c
    ${PROJECT_SOURCE_DIR}/app/*.cpp)

# Removed app file
list(REMOVE_ITEM APP_SOURCES ${PROJECT_SOURCE_DIR}/app/application.cpp)

file(GLOB_RECURSE ARCH_SOURCES
    ${PROJECT_SOURCE_DIR}/arch/stm32f0/*.c
    ${PROJECT_SOURCE_DIR}/arch/stm32f0/*.cpp)

file(GLOB_RECURSE BSP_SOURCES
    ${PROJECT_SOURCE_DIR}/bsp/ctc_v1/*.c
    ${PROJECT_SOURCE_DIR}/bsp/ctc_v1/*.cpp)


set(HWTEST_CTC_V1_DEFINES
    ${CTC_V1_DEFINES}
)