configure_file("version.h.in"
    ${PROJECT_SOURCE_DIR}/app/version.h)

add_custom_target(build_main_targets
    ALL
    DEPENDS
        # emu_ctc_v1
        fw_ctc_v1
    COMMENT "Build main targets"
)

add_custom_target(firmwares
    ALL
    DEPENDS
        fw_ctc_v1
    COMMENT "Build production firmwares"
)