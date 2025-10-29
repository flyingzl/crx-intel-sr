# MiguVideo Super Resolution Chrome Extension

一个使用 Chrome Native Messaging 技术的超分辨率增强扩展，专为咪咕视频提供超分辨率功能，可以让网页通过 `window.nativeApi` 接口获取系统内存信息并启用超分辨率处理。

## 🏗️ 项目结构

```
sr-extension/
├── crx-extension/          # Chrome 扩展代码
│   ├── manifest.json       # 扩展配置文件
│   ├── background.js       # 后台脚本
│   ├── content.js          # 内容脚本
│   ├── injected.js         # 注入脚本（提供 window.nativeApi）
│   ├── popup.html          # 弹窗界面
│   └── popup.js            # 弹窗逻辑
├── sr-native/              # Rust 本地应用
│   ├── Cargo.toml          # Rust 项目配置
│   └── src/main.rs         # 主程序
├── sr-registered/          # Native Messaging Host 注册文件
│   ├── com.miguvideo.sr.windows.json # Windows用的清单文件
│   └── sr.reg              # Windows注册表示例
└── README.md               # 本文档
```

## 🚀 快速开始 (Windows)

### 1. 环境准备

确保你的系统已安装：
- **Rust**: 从官网 [rust-lang.org](https://www.rust-lang.org/tools/install) 下载并运行 `rustup-init.exe` 进行安装。
- **Google Chrome** 浏览器。
- **Git**: 用于克隆代码仓库。

### 2. 安装步骤

#### 第 1 步: 获取代码
打开 PowerShell 或 CMD，克隆本仓库：
```powershell
git clone <repository_url>
cd sr-extension
```

#### 第 2 步: 构建本地应用
编译 Rust 应用。这会在 `sr-native\target\release\` 目录下生成 `sr-native.exe`。
```powershell
cd sr-native
cargo build --release
cd ..
```

#### 第 3 步: 注册 Native Messaging Host
为了让 Chrome 能够找到并与本地应用通信，你需要向 Windows 注册表添加一个项。

1.  **修改清单文件**: 打开 `sr-registered\com.miguvideo.sr.windows.json`，确保 `path` 的值指向你刚刚编译好的 `sr-native.exe` 的 **绝对路径**。
    *   **示例**: `"path": "D:\\codes\\sr-extension\\sr-native\\target\\release\\sr-native.exe"`
    *   **注意**: 在 JSON 文件中，路径分隔符必须使用双反斜杠 `\\`。

2.  **导入注册表文件**:
    *   用文本编辑器打开 `sr-registered\sr.reg` 文件。
    *   将其中的占位符路径替换为 `com.miguvideo.sr.windows.json` 文件的 **绝对路径**。
    *   **示例**: `"@"="D:\\codes\\sr-extension\\sr-registered\\com.miguvideo.sr.windows.json"`
    *   保存 `.reg` 文件，然后双击它，在弹出的确认框中选择“是”，将其导入注册表。

    你也可以使用注册表编辑器 (`regedit.exe`) 手动添加：
    *   导航到 `HKEY_CURRENT_USER\SOFTWARE\Google\Chrome\NativeMessagingHosts\`
    *   新建一个名为 `com.miguvideo.sr` 的项。
    *   将其 `(默认)` 值设置为 `com.miguvideo.sr.windows.json` 文件的完整路径。

#### 第 4 步: 加载 Chrome 扩展
1.  打开 Chrome 浏览器，地址栏输入 `chrome://extensions/` 并访问。
2.  开启右上角的“开发者模式”。
3.  点击“加载已解压的扩展程序”。
4.  选择项目中的 `crx-extension` 文件夹。

现在，扩展的图标应该会出现在 Chrome 的工具栏中。

## 🔧 使用方法

### 在网页中使用 API

扩展会在所有网页中注入 `window.nativeApi` 对象，提供以下方法：

```javascript
// 检查 API 是否可用
if (window.nativeApi && window.nativeApi.isAvailable()) {

  // 启用超分辨率（默认模式0）
  const srResult = await window.nativeApi.turnOnSR(0);
  console.log('SR Enabled:', srResult);

  // 关闭超分辨率
  const srOffResult = await window.nativeApi.turnOffSR();
  console.log('SR Disabled:', srOffResult);

  // 获取超分辨率状态
  const srStatus = await window.nativeApi.getSRStatus();
  console.log('SR Status:', srStatus);

  // 测试连接（包含SR支持信息）
  const pingResult = await window.nativeApi.ping();
  console.log('Ping Result:', pingResult);
}
```

### 使用弹窗测试

点击 Chrome 工具栏中的扩展图标，可以打开测试弹窗，用于测试与本地应用的连接和获取系统信息。

## 🛠️ 开发说明

### Chrome 扩展架构
- **Manifest V3**: 使用最新的扩展清单格式。
- **Content Script**: 向网页注入 API。
- **Background Script**: 处理与本地应用的通信。
- **Popup**: 提供简单的测试界面。

### Native Messaging 流程
1.  网页调用 `window.nativeApi` 方法。
2.  Content Script 将调用转发给 Background Script。
3.  Background Script 通过标准输入/输出（stdin/stdout）与 Rust 应用通信。
4.  Rust 应用处理请求并返回 JSON 格式的结果。
5.  结果通过相同路径返回给网页。

### Rust 应用详情
- **语言**: Rust (2021 Edition)
- **依赖**: `serde`, `serde_json`, `anyhow`, `libc` 等。
- **功能**:
  - 实现 Native Messaging 协议。
  - 集成 SR 库提供超分辨率功能。

### SR 库集成
- **库位置**: `sr-native/sr_lib/`
- **接口**: `SuperResolutionInitialize`, `SuperResolutionSet`, `SuperResolutionGetStatus`, `SuperResolutionRelease`
- **编译配置**: `build.rs` 脚本负责链接 SR 库。

## 🔍 故障排查 (Windows)

### 常见问题

1.  **"Native host has exited" 错误**
    - 通常意味着 Rust 应用崩溃了。
    - 检查 `com.miguvideo.sr.windows.json` 中的 `path` 路径是否正确，并确保使用了双反斜杠 `\\`。
    - 确认 `sr-native.exe` 是否已成功生成在 `sr-native\target\release\` 目录中。

2.  **"Specified native messaging host not found" 错误**
    - 这意味着 Chrome 没能从注册表找到你的应用。
    - 使用 `regedit.exe` 检查注册表项 `HKEY_CURRENT_USER\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.miguvideo.sr` 是否存在。
    - 确保该项的 `(默认)` 值指向了正确的 `.json` 清单文件。

3.  **API 不可用 (`window.nativeApi` 是 undefined)**
    - 确保扩展已在 `chrome://extensions/` 中加载并启用。
    - 检查扩展的后台日志（见下文）是否有错误。

### 调试方法

1.  **查看扩展日志**:
    - 访问 `chrome://extensions/`。
    - 点击扩展的“详细信息”。
    - 点击“服务工作线程”链接，打开后台脚本的开发者控制台。

2.  **调试本地应用**:
    - 本地应用的错误会输出到 `stderr`。由于 Native Messaging 会隐藏这些输出，直接从终端测试是最好的方法。
    - 项目中提供的 `tests/test_native.py` 脚本是最佳调试工具。在项目根目录运行它：
      ```powershell
      python .\sr-native\tests\test_native.py
      ```
    - 这个脚本会模拟 Chrome 发送的消息，并直接显示 Rust 应用的输出和任何错误。

3.  **验证注册表项**:
    - 打开 `regedit.exe` 并导航到 `HKEY_CURRENT_USER\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.miguvideo.sr`。
    - 复制 `(默认)` 值中的路径，尝试在文件浏览器中打开，确保路径无误。

## 📝 许可证

MIT License

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！
