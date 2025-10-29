set(RTT_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/libs/hw/third_party/SEGGER_RTT_V762c/RTT)

# Add sources
file(GLOB_RECURSE RTT_SOURCES
    ${PROJECT_SOURCE_DIR}/libs/hw/third_party/SEGGER_RTT_V762c/RTT/*.c
    ${PROJECT_SOURCE_DIR}/libs/hw/third_party/SEGGER_RTT_V762c/RTT/*.S
    ${PROJECT_SOURCE_DIR}/libs/hw/third_party/SEGGER_RTT_V762c/Syscalls/SEGGER_RTT_Syscalls_GCC.c
)