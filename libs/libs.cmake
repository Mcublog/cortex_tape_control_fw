set(DEBUG_INCLUDES
    ${PROJECT_SOURCE_DIR}/libs/debug
)
file(GLOB_RECURSE DEBUG_SOURCES
    ${PROJECT_SOURCE_DIR}/libs/debug/log_libs.c
)

set(RING_BUFFER_INCLUDES
    ${PROJECT_SOURCE_DIR}/libs/Ring-Buffer
)
file(GLOB_RECURSE RING_BUFFER_SOURCES
    ${PROJECT_SOURCE_DIR}/libs/Ring-Buffer/ringbuffer.c
)

set(SOFTWARE_TIMER_INCLUDES
    ${PROJECT_SOURCE_DIR}/libs/SoftwareTimer
)
file(GLOB_RECURSE SOFTWARE_TIMER_SOURCES
    ${PROJECT_SOURCE_DIR}/libs/SoftwareTimer/SoftTimers.c
)