# FileScanner

基于 Qt 和 C++ 实现的文件扫描器，支持递归扫描指定目录下的所有文件，统计大小并在图形界面中实时展示。

## ✅ 快速启动（无需构建）

如果你仅需运行程序而非开发调试，可以直接使用仓库中提供的可执行程序：

```
Debug/FileScannerUI.exe
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

## 🛠️ 构建说明

本项目依赖 Qt 和 Microsoft Visual Studio 构建工具，以下是推荐的构建流程。

### 1. 准备第三方库

请下载并准备第三方库文件夹 `thirdparty_install`（包含 Qt 相关依赖），下载链接如下：

👉 [thirdparty_install 下载地址](https://365.kdocs.cn/view/l/crFHGQuSLP9Q?openfrom=docs)

请将该文件夹与 `KDevelop-Training/` 项目目录置于同一层级目录中，目录结构如下：

```
root/
├── thirdparty_install/        # 第三方依赖（需手动下载）
└── KDevelop-Training/         # 本项目源码（包含 filescanner/）
    └── filescanner/
```

---

### 2. 创建构建目录

在项目根目录下创建并进入构建目录：

```bash
mkdir Debug
cd Debug
```

---

### 3. 配置 Visual Studio 环境

使用 Microsoft Visual Studio 提供的命令行环境进行编译。在命令行中调用对应版本的编译环境（以 Visual Studio 2019 为例）：

```cmd
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsamd64_x86.bat"
```

> 请根据你的实际 Visual Studio 安装路径和版本适当调整上述命令。

---

### 4. 执行构建命令

根据需要选择构建 Debug 或 Release 版本：

#### 构建 Debug 版本：

```cmd
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Debug ../KDevelop-Training
msbuild /m KDevelop-Train.sln /p:Platform=x64 /p:Configuration=Debug
```

#### 构建 Release 版本：

```cmd
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release ../KDevelop-Training
msbuild /m KDevelop-Train.sln /p:Platform=x64 /p:Configuration=Release
```

---

构建完成后，可在 `Debug/` 或 `Release/` 目录中找到可执行程序 `FileScannerUI.exe`，双击运行即可。

