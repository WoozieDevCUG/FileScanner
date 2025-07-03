# FileScanner

基于 Qt 和 C++ 实现的文件扫描器，支持递归扫描指定目录下的所有文件，统计大小并在图形界面中实时展示。

---

## 📁 项目结构

```plaintext
/
├── KDevelop-Training/              # 主项目目录
│   └── filescanner/                # 核心模块（全小写）
│       ├── CMakeLists.txt          # 主构建配置
│       │
│       ├── FileScanner/           # ▶核心库（DLL）
│       │   ├── CMakeLists.txt      # 库构建配置
│       │   ├── FileScanner.h      # 🔷 QObject派生类（含Q_OBJECT）
│       │   └── FileScanner.cpp    # 🔶 扫描逻辑实现
│       │
│       └── FileScannerUI/         # ▶测试程序
│           ├── CMakeLists.txt      # UI构建配置
│           ├── main.cpp           # ⚙️ 程序入口
│           ├── MainWindow.h       # 🔷 主窗口类
│           └── MainWindow.cpp     # 🔶 UI逻辑实现
│
└── thirdparty_install/            # 第三方库目录
    ├── qt5-tools/                   
    └── vcpkg/       
```

## 功能特性

- 扫描指定目录及其子目录中的所有文件
- 实时更新文件扫描进度
- 显示文件路径和大小信息
- 支持中断扫描
- DLL 动态调用与主程序解耦

## 技术栈

- **C++**
- **Qt 5**（Widgets、Core、Concurrent）
- **Windows API**（`LoadLibrary`，`GetProcAddress` 等）
- **CMake** 构建系统
- **多线程**（使用 `QtConcurrent` 和 `QThreadPool`）
- **回调函数**实现 DLL 到 UI 的事件通知

          
