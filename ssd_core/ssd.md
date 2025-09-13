ssd_nuro/
├── core/
│   ├── ssd_core.h          # SSD核心定義（依存なし）
│   ├── ssd_core.cpp        # SSD実装
│   ├── neuro_core.h        # 神経モデル（独立）
│   └── neuro_core.cpp      # 神経実装
├── bridge/
│   ├── neuro_ssd_bridge.h  # 連携インターface
│   └── neuro_ssd_bridge.cpp# 連携実装
├── api/
│   ├── ssd_api.h           # 外部公開API
│   └── ssd_api.cpp         # APIラッパー
└── CMakeLists.txt