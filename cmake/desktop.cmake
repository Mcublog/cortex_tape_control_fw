set(DESKTOP_TARGET                  emu_ctc_v1)

set(IO_MOCK_DIR     ${DESKTOP_TARGET_PATH}/io_mock)
set(MOCKS_DIR       ${DESKTOP_TARGET_PATH}/mocks)

set(DESKTOP_INCLUDE_DIR
    ${DESKTOP_DIR}
    ${IO_MOCK_DIR}
    ${MOCKS_DIR}
    ${APP_DIRS}
)

file(GLOB_RECURSE BSP_SOURCES
    ${PROJECT_SOURCE_DIR}/bsp/desktop/*.c
    ${PROJECT_SOURCE_DIR}/bsp/desktop/*.cpp
)

file(GLOB_RECURSE ARCH_SOURCES
    ${PROJECT_SOURCE_DIR}/arch/desktop/*.c
    ${PROJECT_SOURCE_DIR}/arch/desktop/*.cpp
)

set(EMU_DEFINES
    $<$<CONFIG:DEBUG>:"DEBUG">
    $<$<CONFIG:RELASE>:"NDEBUG">
    $<$<CONFIG:RelWithDebInfo>:"FORCE_LOG">
    "DESKTOP"
    "MAX_TIMERS=10"
)