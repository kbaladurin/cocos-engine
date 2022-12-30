macro(cc_openharmony_before_target target_name)
    list(APPEND CC_COMMON_SOURCES
        ${COCOS_X_PATH}/cocos/platform/openharmony/napi/NapiInit.cpp
    )

    if(USE_ETS)
        list(APPEND CC_COMMON_SOURCES
            ${COCOS_X_PATH}/cocos/platform/openharmony/napi/NapiInit.cpp
        )
    endif()

    set(CC_ALL_SOURCES ${CC_COMMON_SOURCES} ${CC_PROJ_SOURCES})
    cc_common_before_target(${target_name})
endmacro()

macro(cc_openharmony_after_target target_name)

    target_link_libraries(${target_name}
        ${ENGINE_NAME}
    )
    target_include_directories(${target_name} PRIVATE
        ${CC_PROJECT_DIR}/../common/Classes
    )

    if(USE_ETS)
        if(NOT ARK_ROOT)
            message(FATAL_ERROR "ARK_ROOT is not set!")
        endif()

        if(NOT ARK_BUILD_ROOT)
            message(FATAL_ERROR "ARK_BUILD_ROOT is not set!")
        endif()

        target_include_directories(${target_name} PUBLIC
            ${ARK_ROOT}
            ${ARK_ROOT}/libpandabase
        )

        target_link_directories(${target_name} PUBLIC
            ${ARK_BUILD_ROOT}/lib
        )

        target_link_libraries(${target_name} PUBLIC
            arkruntime
        )
    endif()

    cc_common_after_target(${target_name})
endmacro()