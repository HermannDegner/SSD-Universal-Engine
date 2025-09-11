# SSD Universal Engine - ビルド手順

構造主観力学（SSD）汎用評価エンジンのコンパイル・ビルド手順です。

## 必要ファイル一覧

以下のファイルが同一ディレクトリに必要です：

### 必須ファイル
```
CMakeLists.txt                    # CMakeビルド設定
ssd_align_leap_dll.h             # 基本SSDコア API
ssd_align_leap_dll.cpp           # 基本SSDコア 実装
ssd_universal_engine_dll.h       # 汎用エンジン API
ssd_universal_engine_dll.cpp     # 汎用エンジン 実装
```

### テストファイル
```
test_basic.cpp                   # 基本機能テスト
test_npc.cpp                     # NPC行動テスト
test_c_api.c                     # C API互換性テスト
benchmark.cpp                    # パフォーマンステスト
```

## 必要な開発環境

### Windows
- **Visual Studio 2019以降** または **MinGW-w64**
- **CMake 3.12以降**
- **C++17対応コンパイラ**

### Linux/Ubuntu
```bash
sudo apt update
sudo apt install build-essential cmake g++ git
```

### macOS
```bash
# Homebrewが必要
brew install cmake
# Xcode Command Line Toolsが必要
xcode-select --install
```

## ビルド手順

### 1. 基本ビルド（推奨）

```bash
# ビルドディレクトリ作成
mkdir build
cd build

# CMake設定（Release版）
cmake .. -DCMAKE_BUILD_TYPE=Release

# コンパイル実行
cmake --build . --config Release

# テスト実行
./ssd_test_basic
./ssd_test_npc
./ssd_test_c_api
./ssd_benchmark
```

### 2. Windows（Visual Studio）

```cmd
REM ビルドディレクトリ作成
mkdir build
cd build

REM Visual Studio プロジェクト生成
cmake .. -G "Visual Studio 16 2019" -A x64

REM ビルド実行
cmake --build . --config Release

REM テスト実行
Release\ssd_test_basic.exe
Release\ssd_test_npc.exe
Release\ssd_test_c_api.exe
Release\ssd_benchmark.exe
```

### 3. デバッグビルド

```bash
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

## ビルドオプション

### 利用可能なオプション

```bash
# テスト無効化
cmake .. -DBUILD_TESTS=OFF

# Python バインディング（要pybind11）
cmake .. -DBUILD_PYTHON_BINDINGS=ON

# C# バインディング
cmake .. -DBUILD_CSHARP_BINDINGS=ON

# Unity パッケージ生成
cmake .. -DBUILD_UNITY_PACKAGE=ON

# ドキュメント生成（要Doxygen）
cmake .. -DBUILD_DOCUMENTATION=ON

# 全オプション有効
cmake .. -DBUILD_TESTS=ON -DBUILD_PYTHON_BINDINGS=ON -DBUILD_CSHARP_BINDINGS=ON
```

## 生成される成果物

### ライブラリ
- **Windows**: `ssd_universal_engine.dll`
- **Linux**: `libssd_universal_engine.so`
- **macOS**: `libssd_universal_engine.dylib`

### テスト実行ファイル
- `ssd_test_basic` - 基本機能テスト
- `ssd_test_npc` - NPC行動分析テスト
- `ssd_test_c_api` - C API互換性テスト
- `ssd_benchmark` - パフォーマンス測定

## トラブルシューティング

### よくあるエラー

#### 1. C++17コンパイラエラー
```
CMake Error: CMAKE_CXX_COMPILER could not find C++17
```

**解決方法**: より新しいコンパイラをインストール
```bash
# Ubuntu/Debian
sudo apt install g++-9

# CentOS/RHEL
sudo yum install gcc-toolset-9

# macOS
brew install gcc
```

#### 2. CMakeバージョンエラー
```
CMake Error: CMake 3.12 or higher is required
```

**解決方法**: CMakeを更新
```bash
# Ubuntu（snapを使用）
sudo snap install cmake --classic

# macOS
brew upgrade cmake

# Windows
# CMake公式サイトから最新版をダウンロード
```

#### 3. リンクエラー（Linux）
```
undefined reference to pthread_create
```

**解決方法**: pthreadライブラリが自動リンクされるはずですが、手動で追加する場合：
```bash
cmake .. -DCMAKE_EXE_LINKER_FLAGS="-pthread"
```

### パフォーマンス最適化

#### 最高性能ビルド
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"
```

#### デバッグ情報付きリリース
```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## テスト実行と結果確認

### 基本テスト
```bash
./ssd_test_basic
```
期待する出力:
```
=== Engine Lifecycle Test ===
Engine created successfully
Version: SSD Universal Engine v1.0.0
...
All tests PASSED! ✅
```

### NPCテスト
```bash
./ssd_test_npc
```
期待する出力:
```
=== Villager Daily Routine Test ===
住民「リリアン」の行動慣性分析
...
All NPC tests PASSED! 🎮✅
```

### パフォーマンステスト
```bash
./ssd_benchmark
```
期待する出力:
```
🚀 Basic System Evaluation Benchmark
Running 1000 evaluations...
System Evaluation Performance:
  Average: 0.123 ms
  Rate: 8130.081 ops/sec
...
All benchmarks COMPLETED! 🚀✅
```

## 使用方法

### C++での使用例

```cpp
#include "ssd_universal_engine_dll.h"

int main() {
    // エンジン作成
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    
    // NPC行動評価
    double basal_drives[5] = {0.3, 0.8, 0.4, 0.2, 0.6};
    double routine_strengths[4] = {0.9, 0.7, 0.5, 0.3};
    double episodic_influences[3] = {0.8, -0.2, 0.6};
    double environmental_factors[4] = {0.9, 0.7, 0.5, 0.8};
    
    double action_inertia, confidence;
    char reasoning[512];
    
    SSDReturnCode result = ssd_evaluate_npc_action(
        engine, "greet_player", "PlayerA",
        basal_drives, 5, routine_strengths, 4,
        episodic_influences, 3, environmental_factors, 4,
        &action_inertia, &confidence, reasoning, sizeof(reasoning)
    );
    
    if (result == SSD_SUCCESS) {
        printf("Action Inertia: %.3f\n", action_inertia);
        printf("Confidence: %.3f\n", confidence);
        printf("Reasoning: %s\n", reasoning);
    }
    
    // エンジン破棄
    ssd_universal_destroy(engine);
    return 0;
}
```

### Cでの使用例

```c
#include "ssd_universal_engine_dll.h"
#include <stdio.h>

int main() {
    // エンジン作成
    SSDUniversalEngine* engine = ssd_universal_create(NULL);
    
    // バージョン情報表示
    printf("Version: %s\n", ssd_get_version_string());
    
    // 統計情報取得
    SSDEngineStats stats;
    ssd_universal_get_stats(engine, &stats);
    printf("Engine ID: %s\n", stats.engine_id);
    
    // エンジン破棄
    ssd_universal_destroy(engine);
    return 0;
}
```

## インストール（オプション）

システム全体にインストールする場合：

```bash
# ビルド後、インストール実行
sudo cmake --install . --config Release

# pkg-configで確認
pkg-config --cflags --libs ssd_universal_engine
```

## 追加情報

- **API リファレンス**: `ssd_universal_engine_dll.h` のコメントを参照
- **理論的背景**: [構造主観力学（SSD）理論文書](README.md) を参照
- **パフォーマンス**: ベンチマークで 8000+ ops/sec (基本評価) を確認
- **メモリ使用量**: 典型的に < 10MB (キャッシュ含む)

## サポート

ビルドに関する問題がある場合は、まず以下を確認してください：

1. CMakeバージョンが3.12以降か
2. C++17対応コンパイラが利用可能か
3. 必要なファイルが全て揃っているか
4. 適切な権限でビルドディレクトリにアクセス可能か

正常にビルドが完了すると、SSDの理論的枠組みを実際のプログラムで利用できるようになります。