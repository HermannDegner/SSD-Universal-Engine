cmake_minimum_required(VERSION 3.12)
project(ssd_universal_engine VERSION 1.0.0 LANGUAGES CXX)

# C++17標準を要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ビルド設定
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# コンパイラ別フラグ設定
if(MSVC)
    # MSVC用フラグ
    set(CMAKE_CXX_FLAGS "/W3")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MD /DNDEBUG")
else()
    # GCC/Clang用フラグ
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# ソースファイル
set(CORE_SOURCES
    ssd_align_leap_dll.cpp
    ssd_universal_engine_dll.cpp
)

set(CORE_HEADERS
    ssd_align_leap_dll.h
    ssd_universal_engine_dll.h
)

# 共有ライブラリ（DLL）のビルド
add_library(ssd_universal_engine SHARED ${CORE_SOURCES})

# ヘッダーファイルのインクルードディレクトリ
target_include_directories(ssd_universal_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Windows固有の設定
if(WIN32)
    set_target_properties(ssd_universal_engine PROPERTIES
        OUTPUT_NAME "ssd_universal_engine"
        WINDOWS_EXPORT_ALL_SYMBOLS ON
        PREFIX ""
        SUFFIX ".dll"
    )
    
    # Windows DLLエクスポート定義
    target_compile_definitions(ssd_universal_engine PRIVATE SSD_UNIVERSAL_DLL_EXPORTS)
    target_compile_definitions(ssd_universal_engine PRIVATE SSD_ALIGN_LEAP_DLL_EXPORTS)
endif()

# Linux/Unix固有の設定
if(UNIX)
    set_target_properties(ssd_universal_engine PROPERTIES
        OUTPUT_NAME "ssd_universal_engine"
        PREFIX "lib"
        SUFFIX ".so"
        VERSION "1.0.0"
        SOVERSION "1"
    )
    
    # Position Independent Code
    set_target_properties(ssd_universal_engine PROPERTIES POSITION_INDEPENDENT_CODE ON)
    
    # Linux特有のライブラリリンク
    target_link_libraries(ssd_universal_engine PRIVATE pthread dl)
endif()

# macOS固有の設定
if(APPLE)
    set_target_properties(ssd_universal_engine PROPERTIES
        OUTPUT_NAME "ssd_universal_engine"
        PREFIX "lib"
        SUFFIX ".dylib"
        MACOSX_RPATH ON
        INSTALL_NAME_DIR "@rpath"
    )
endif()

# コンパイラ別最適化設定
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ssd_universal_engine PRIVATE
        -ffast-math
        -fno-exceptions
        -fvisibility=hidden
    )
    
    # リリースビルドでのLTO
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(ssd_universal_engine PRIVATE -flto)
        target_link_options(ssd_universal_engine PRIVATE -flto)
    endif()
    
elseif(MSVC)
    target_compile_options(ssd_universal_engine PRIVATE
        /Oi          # 組み込み関数の有効化
        /Ot          # 速度優先最適化
        /GS-         # セキュリティチェック無効化（パフォーマンス向上）
    )
    
    # リリースビルドでのLTO
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(ssd_universal_engine PRIVATE /GL)
        target_link_options(ssd_universal_engine PRIVATE /LTCG)
    endif()
endif()

# テストプログラム（オプション）
option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    # シンプルなテストプログラム
    add_executable(ssd_test_basic test_basic.cpp)
    target_link_libraries(ssd_test_basic PRIVATE ssd_universal_engine)
    
    # NPCテストプログラム
    add_executable(ssd_test_npc test_npc.cpp)
    target_link_libraries(ssd_test_npc PRIVATE ssd_universal_engine)
    
    # パフォーマンステスト
    add_executable(ssd_benchmark benchmark.cpp)
    target_link_libraries(ssd_benchmark PRIVATE ssd_universal_engine)
    
    # C互換性テスト
    add_executable(ssd_test_c_api test_c_api.c)
    target_link_libraries(ssd_test_c_api PRIVATE ssd_universal_engine)
    set_target_properties(ssd_test_c_api PROPERTIES
        LINKER_LANGUAGE CXX  # C++ライブラリをリンクするため
    )
    
    # テスト実行ファイルにも適切な設定を適用
    if(MSVC)
        target_compile_options(ssd_test_basic PRIVATE /W3)
        target_compile_options(ssd_test_npc PRIVATE /W3)
        target_compile_options(ssd_benchmark PRIVATE /W3)
        target_compile_options(ssd_test_c_api PRIVATE /W3)
    endif()
endif()

# 基本的なインストールルール（オプション）
option(ENABLE_INSTALL "Enable install rules" OFF)

if(ENABLE_INSTALL)
    install(TARGETS ssd_universal_engine
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
    
    install(FILES ${CORE_HEADERS} DESTINATION include)
endif()

# 設定サマリーの表示
message(STATUS "=== SSD Universal Engine Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Enable install: ${ENABLE_INSTALL}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
if(MSVC)
    message(STATUS "MSVC Version: ${MSVC_VERSION}")
endif()
message(STATUS "===============================================")