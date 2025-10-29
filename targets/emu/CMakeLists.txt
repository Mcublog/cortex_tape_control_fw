include(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/desktop.cmake)
include(${PROJECT_SOURCE_DIR}/libs/libs.cmake)

# For testing using desktop gcc compiler
set(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)
# Add coloring
add_compile_options(-fdiagnostics-color=always)
# Set defines
add_definitions(-DprojCOVERAGE_TEST=0)
add_definitions(-D_WINDOWS_)
add_definitions(-DDESKTOP)

# Add sources
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Buid DESKTOP target
add_executable(${DESKTOP_TARGET}
    ${SOURCES}
    ${APP_SOURCES}
    ${ARCH_SOURCES}
    ${BSP_SOURCES}
    # Libs
    ${SOFTWARE_TIMER_SOURCES}
    ${CAPACITY_METER_SOURCES}
    ${RING_BUFFER_SOURCES}
    ${DEBUG_SOURCES}
    ${RINGFS_SOURCES}
    ${COBS_SOURCES}
)

# Include paths
target_include_directories(${DESKTOP_TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${APP_DIR}
    ${SOFTWARE_TIMER_INCLUDES}
    ${RING_BUFFER_INCLUDES}
    ${CAPACITY_METER_INCLUDES}
    ${DEBUG_INCLUDES}
    ${LIBS_DIR}
    ${RINGFS_INCLUDES}
)

# Project symbols
target_compile_definitions(${DESKTOP_TARGET} PRIVATE ${EMU_DEFINES})

# Add pthread
target_link_libraries(${DESKTOP_TARGET} pthread)
