# ModernUI

**C++20 现代 GPU 加速 GUI 工具包** — 基于 OpenGL 3.3 Core Profile，支持 CJK 动态字体渲染、主题切换动画、全部组件右键菜单、多线程布局。

---

## 特性

- **40+ 组件** — Button / Label / TextInput / TextEdit / Dropdown / Select / TabView / ScrollArea / Table / Menu / DatePicker / BarChart / StatCard / Card / Badge / Dialog / MessageBubble / Toast / Tooltip / Switch / Checkbox / Radio / Slider / ProgressBar / Loading / Pagination / CollapsePanel / Form / Border / Link / InputNumber / Image / Avatar / MessageBox / ContextMenu 等
- **主题切换动画** — 暗色/亮色 0.4s 平滑过渡，全部 ColorRole/FontSize/Spacing/BorderRadius 同步插值
- **CJK 动态字体** — Unicode 码点按需光栅化，中文/日文/韩文/Emoji，多图集页自动扩展
- **右键菜单** — TextInput / TextEdit / Label 支持 复制/粘贴/剪切/删除/全选/撤销/重做
- **文本选中** — 拖拽选中 + Shift 扩展 + 选中高亮 + UTF-8 码点级光标
- **全局配置** — `Config` 单例集中管理所有可调参数
- **可选多线程** — `ThreadPool` 并行 `measure()` 布局计算
- **45 种图标** — 纯几何矢量图标，Button/CollapsePanel/Toast/Checkbox 均可设置
- **现代化样式** — 圆角、阴影、主题色变体、尺寸变体覆盖全部组件
- **UTF-8 全链路** — 输入/删除/光标/换行/测量 全部码点感知

## 快速开始

### 环境要求

- Windows x64 / Visual Studio 2022
- CMake 3.14+ / OpenGL 3.3

### 构建

```bash
cd modernui
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

输出：
- `build/Release/modernui_static.lib` — 静态库
- `build/Release/modernui_shared.dll` + `.lib` — 动态库
- `build/Release/exp.exe` — 组件测试
- `build/Release/test_demo.exe` — 完整 Demo
- `build/Release/test_cjk.exe` — CJK 字体测试
- `build/Release/test_unit.exe` — 单元测试

### 安装（发布打包）

```bash
cmake --install . --prefix ./install --config Release
```

目录结构：
```
install/
├── include/modernui/   # 头文件
├── include/glad/       # OpenGL Loader
├── include/stb/        # stb 单文件库
├── include/glm/        # GLM 数学库
├── lib/
│   ├── modernui_static.lib
│   ├── modernui_shared.lib
│   └── glfw3.lib
└── bin/
    ├── modernui_shared.dll
    └── glfw3.dll
```

### 最小程序

```cpp
#include "core/App.hpp"
#include "widgets/Container.hpp"
#include "widgets/Label.hpp"

int main() {
    ui::App app;
    app.init("Hello", 400, 300);

    auto label = std::make_unique<ui::Label>("你好，ModernUI！");
    label->setFont(app.getFont());

    auto root = std::make_unique<ui::Container>();
    root->addChild(std::move(label));
    app.setRoot(std::move(root));
    app.run();
}
```

## 架构

```
src/
├── core/           # App, Window, Painter, QuadBatcher, Event, Animation, Config
├── res/            # Shader, Texture, Font, Theme, BlurShader, StyleSheet
├── widgets/        # 全部组件
└── utils/          # AsyncTask, HttpClient, Clipboard, ThreadPool, Navigation
```

## 组件列表

| 类别 | 组件 |
|------|------|
| 基础 | Button, Label, TextInput, TextEdit, Switch, Checkbox, RadioButton |
| 容器 | Container(Row/Column), ScrollArea, TabView, Card, Border, CollapsePanel, Form |
| 选择 | Dropdown, Select, Slider, InputNumber, DatePicker |
| 数据 | Table, Pagination, ProgressBar, StatCard, BarChart |
| 导航 | NavMenu, Link |
| 弹窗 | Dialog, MessageBox, ContextMenu, Toast, Tooltip |
| 装饰 | Badge, Avatar, Image, LoadingSpinner, Divider, Spacer |
| 消息 | MessageBubble(聊天气泡) |

## 主题

```cpp
// 切换主题（0.5s 动画过渡）
Config::instance().darkMode = false;
g_transition.setTarget(Theme::defaultLight(), 0.5f);

// 自定义主题色
Theme t;
t.setColor(ColorRole::Primary, Color::fromHex(0xFF6366F1));
t.setColor(ColorRole::Success, Color::fromHex(0xFF22C55E));
```

## 配置

```cpp
auto& cfg = Config::instance();
cfg.multiThreaded = true;        // 开启多线程布局
cfg.themeTransitionDuration = 0.5f;
cfg.scrollStep = 50.0f;
cfg.loadFromFile("modernui.cfg"); // 从文件加载
cfg.saveToFile("modernui.cfg");   // 保存到文件
```

## 许可证

MIT License (c) 2026 ModernUI

## 链接

- GitHub: [https://github.com/PogiePop/modern-ui](https://github.com/PogiePop/modern-ui)
