cmake_minimum_required(VERSION 3.12)
project(ssd_universal_engine LANGUAGES CXX)

# C++17標準を要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ビルド設定
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# コンパイラフラグ
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

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
    
    # Windows特有のライブラリリンク
    target_link_libraries(ssd_universal_engine PRIVATE ws2_32)
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

# 最適化とパフォーマンス設定
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ssd_universal_engine PRIVATE
        -march=native
        -ffast-math
        -fno-exceptions  # 例外を使わない場合
        -fvisibility=hidden
    )
    
    # リリースビルドでのLTO（Link Time Optimization）
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(ssd_universal_engine PRIVATE -flto)
        target_link_options(ssd_universal_engine PRIVATE -flto)
    endif()
endif()

if(MSVC)
    target_compile_options(ssd_universal_engine PRIVATE
        /O2          # 最適化
        /Oi          # 組み込み関数の有効化
        /Ot          # 速度優先最適化
        /GL          # プログラム全体の最適化
    )
    
    # リリースビルドでのリンク時最適化
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
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
endif()

# Python バインディング（オプション）
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)

if(BUILD_PYTHON_BINDINGS)
    find_package(pybind11 QUIET)
    if(pybind11_FOUND)
        pybind11_add_module(ssd_python ssd_python_bindings.cpp)
        target_link_libraries(ssd_python PRIVATE ssd_universal_engine)
        target_compile_definitions(ssd_python PRIVATE VERSION_INFO=${PROJECT_VERSION})
    else()
        message(WARNING "pybind11 not found. Python bindings will not be built.")
    endif()
endif()

# C# バインディング（オプション）
option(BUILD_CSHARP_BINDINGS "Build C# bindings" OFF)

if(BUILD_CSHARP_BINDINGS)
    # C# P/Invoke用のラッパー生成
    add_custom_target(csharp_bindings
        COMMAND ${CMAKE_COMMAND} -E echo "Generating C# bindings..."
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/generate_csharp_bindings.py 
                ${CMAKE_CURRENT_SOURCE_DIR}/ssd_universal_engine_dll.h
                ${CMAKE_CURRENT_BINARY_DIR}/SSDUniversalEngine.cs
        DEPENDS ssd_universal_engine_dll.h
        COMMENT "Generating C# P/Invoke bindings"
    )
endif()

# Unity Package（オプション）
option(BUILD_UNITY_PACKAGE "Build Unity package" OFF)

if(BUILD_UNITY_PACKAGE)
    # Unity用のパッケージ構造を作成
    set(UNITY_PACKAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/UnityPackage)
    
    add_custom_target(unity_package
        COMMAND ${CMAKE_COMMAND} -E make_directory ${UNITY_PACKAGE_DIR}/Plugins
        COMMAND ${CMAKE_COMMAND} -E make_directory ${UNITY_PACKAGE_DIR}/Scripts
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ssd_universal_engine> ${UNITY_PACKAGE_DIR}/Plugins/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/unity/SSDUniversalEngine.cs ${UNITY_PACKAGE_DIR}/Scripts/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/unity/package.json ${UNITY_PACKAGE_DIR}/
        DEPENDS ssd_universal_engine
        COMMENT "Creating Unity package"
    )
endif()

# ドキュメント生成（オプション）
option(BUILD_DOCUMENTATION "Build documentation" OFF)

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    else()
        message(WARNING "Doxygen not found. Documentation will not be built.")
    endif()
endif()

# インストールルール
install(TARGETS ssd_universal_engine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

# ヘッダーファイルのインストール
install(FILES ${CORE_HEADERS} DESTINATION include)

# pkg-configファイルの生成とインストール
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ssd_universal_engine.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/ssd_universal_engine.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ssd_universal_engine.pc 
        DESTINATION lib/pkgconfig)

# CMake Config ファイルの生成
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/SSDUniversalEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SSDUniversalEngineConfig.cmake
    INSTALL_DESTINATION lib/cmake/SSDUniversalEngine
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SSDUniversalEngineConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SSDUniversalEngineConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SSDUniversalEngineConfigVersion.cmake
    DESTINATION lib/cmake/SSDUniversalEngine
)

# CPack設定（パッケージ生成）
set(CPACK_PACKAGE_NAME "SSDUniversalEngine")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Structural Subjectivity Dynamics Universal Evaluation Engine")
set(CPACK_PACKAGE_VENDOR "SSD Project")
set(CPACK_PACKAGE_CONTACT "ssd@example.com")

# プラットフォーム別パッケージ設定
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "SSD Universal Engine")
    set(CPACK_NSIS_PACKAGE_NAME "SSDUniversalEngine")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://example.com/ssd")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "SSD Universal Engine")
else()
    set(CPACK_GENERATOR "ZIP;TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "SSD Project <ssd@example.com>")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# 使用例とサンプルコード
install(DIRECTORY examples/ DESTINATION share/ssd_universal_engine/examples
        FILES_MATCHING PATTERN "*.cpp" PATTERN "*.c" PATTERN "*.h" PATTERN "*.py" PATTERN "*.cs")

# 設定サマリーの表示
message(STATUS "=== SSD Universal Engine Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "Build C# bindings: ${BUILD_CSHARP_BINDINGS}")
message(STATUS "Build Unity package: ${BUILD_UNITY_PACKAGE}")
message(STATUS "Build documentation: ${BUILD_DOCUMENTATION}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "===============================================")