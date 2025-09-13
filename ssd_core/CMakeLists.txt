cmake_minimum_required(VERSION 3.12)
project(ssd_unified_engine LANGUAGES CXX)

# 段階1: SSDコアのみ（テスト用）
add_library(ssd_core_only SHARED
    core/ssd_core.cpp
)
target_include_directories(ssd_core_only PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(ssd_core_only PRIVATE cxx_std_17)
target_compile_definitions(ssd_core_only PRIVATE SSD_CORE_EXPORTS)

set_target_properties(ssd_core_only PROPERTIES
    OUTPUT_NAME "ssd_core_only"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# 段階2: NeuroCorを追加したい場合（オプション）
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/neuro_core.cpp")
    add_library(neuro_core STATIC
        core/neuro_core.cpp
    )
    target_include_directories(neuro_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(neuro_core PRIVATE cxx_std_17)
endif()

# 段階3: ブリッジを追加したい場合（オプション）
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bridge/neuro_ssd_bridge.cpp" AND TARGET neuro_core)
    add_library(neuro_ssd_bridge STATIC
        bridge/neuro_ssd_bridge.cpp
    )
    target_link_libraries(neuro_ssd_bridge PUBLIC ssd_core_only neuro_core)
    target_include_directories(neuro_ssd_bridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(neuro_ssd_bridge PRIVATE cxx_std_17)
endif()

# 段階4: 完全版（全ファイルが揃ったら）
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/api/ssd_api.cpp" AND TARGET neuro_ssd_bridge)
    add_library(ssd_unified_engine SHARED
        api/ssd_api.cpp
    )
    target_link_libraries(ssd_unified_engine PUBLIC neuro_ssd_bridge)
    target_compile_definitions(ssd_unified_engine PRIVATE SSD_API_EXPORTS)
    target_compile_features(ssd_unified_engine PRIVATE cxx_std_17)

    set_target_properties(ssd_unified_engine PROPERTIES
        OUTPUT_NAME "ssd_unified_engine"
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

# インストール設定
install(TARGETS ssd_core_only 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES core/ssd_core.h DESTINATION include)

# 追加ファイルがあればインストール
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/neuro_core.h")
    install(FILES core/neuro_core.h DESTINATION include)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/api/ssd_api.h" AND TARGET ssd_unified_engine)
    install(FILES api/ssd_api.h DESTINATION include)
    install(TARGETS ssd_unified_engine
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
endif()