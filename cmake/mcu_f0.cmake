# region HAL
set(QM_V1_HAL_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/Core/Inc
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/Drivers/STM32F0xx_HAL_Driver/Inc
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/Drivers/CMSIS/Device/ST/STM32F0xx/Include
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/Drivers/CMSIS/Include
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/USB_DEVICE/Target
    ${PROJECT_SOURCE_DIR}/targets/ctc_v1/USB_DEVICE/App
)
# endregion

# Put here your symbols (preprocessor defines), one in each line
# Encapsulate them with double quotes for safety purpose
set(QM_V1_DEFINES
    $<$<CONFIG:DEBUG>:"DEBUG">
    $<$<CONFIG:RELASE>:"NDEBUG">
    $<$<CONFIG:RelWithDebInfo>:"FORCE_LOG">
    "USE_HAL_DRIVER"
    "STM32F072xB"
    "MAX_TIMERS=10"
)

set(CPU_PARAMETERS -mthumb
    # This needs attention to properly set for used MCU
    -mcpu=cortex-m0
)

set(ARM_NONE_EABI_FLAGS
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    -fdata-sections
    -ffunction-sections
    -fdiagnostics-color=always
    # Full debug configuration
    $<$<CONFIG:DEBUG>: -Og -g3 -ggdb>
    # Release
    $<$<CONFIG:RELEASE>:-Os -s>
    # Release with DEBUG
    $<$<CONFIG:RelWithDebInfo>:-Os -g3 -ggdb>
)

# Set linker script
set(F072_LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/targets/ctc_v1/STM32F072CBTx_FLASH.ld)

set(F072_LINKER_OPTION
    -T${F072_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -specs=nano.specs
    -specs=nosys.specs
    # -u _printf_float                # STDIO float formatting support
    -Wl,--start-group
    -lc
    -lm
    -fno-exceptions
    -fno-rtti
    -Wl,--end-group
    -Wl,--gc-sections # removed unused functions
    -Wl,--print-memory-usage
)
# endregion