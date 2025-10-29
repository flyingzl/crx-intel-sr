# Windows 安装说明

## 📋 Windows 上安装 MiguVideo Super Resolution 扩展

### 1. 准备工作

确保你已经：
- 安装了 Google Chrome 浏览器
- 编译了 `sr-native.exe` 程序（Rust 项目）
- 获取了 Chrome 扩展的 Extension ID

### 2. 安装步骤

#### 步骤 1: 更新配置文件
1. 打开 `com.miguvideo.sr.windows.json` 文件
2. 将 `YOUR_EXTENSION_ID_HERE` 替换为你的实际扩展ID
3. 确认 `path` 路径指向正确的 `sr-native.exe` 位置

#### 步骤 2: 注册 Native Messaging Host
**方法一：使用注册表文件（推荐）**
1. 双击 `sr.reg` 文件
2. 在弹出的对话框中点击"是"确认导入

**方法二：手动注册**
1. 按 `Win + R` 打开运行对话框
2. 输入 `regedit` 并回车
3. 导航到 `HKEY_CURRENT_USER\SOFTWARE\Google\Chrome\NativeMessagingHosts\`
4. 创建新项 `com.miguvideo.sr`
5. 设置默认值为 JSON 配置文件的完整路径

#### 步骤 3: 加载 Chrome 扩展
1. 打开 Chrome 浏览器
2. 访问 `chrome://extensions/`
3. 开启"开发者模式"
4. 点击"加载已解压的扩展程序"
5. 选择 `crx-extension` 文件夹

### 3. 验证安装

1. 在 Chrome 中访问 `https://www.miguvideo.com/`
2. 打开开发者工具（F12）
3. 在控制台中输入：
   ```javascript
   window.nativeApi.ping()
   ```
4. 如果返回成功响应，说明安装正确

### 4. 故障排除

#### 常见错误：
- **"Specified native messaging host not found"**
  - 检查注册表项是否正确创建
  - 确认 JSON 配置文件路径是否正确
  - 验证 `sr-native.exe` 是否存在且可执行

- **"Native host has exited"**
  - 检查 `sr-native.exe` 是否有执行权限
  - 查看 Chrome 错误日志：启动 Chrome 时添加 `--enable-logging --v=1` 参数

#### 调试方法：
1. 手动测试 native 程序：
   ```cmd
   echo {"action":"ping"} | sr-native.exe
   ```

2. 检查注册表：
   ```cmd
   reg query "HKEY_CURRENT_USER\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.miguvideo.sr"
   ```

### 5. 卸载

如需卸载，删除注册表项：
```cmd
reg delete "HKEY_CURRENT_USER\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.miguvideo.sr" /f
```

---

💡 **提示**：如果需要为所有用户安装，请以管理员身份运行，并使用 `HKEY_LOCAL_MACHINE` 注册表项。