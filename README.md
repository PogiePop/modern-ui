# ModernUI

A modern C++20 retained-mode UI component library built on OpenGL 3.3 Core Profile.

## Features

- **Retained Mode** widget tree with `unique_ptr` ownership
- **50+ widgets**: Button, Label, TextInput, TextEdit, Checkbox, RadioButton, Slider, ScrollBar, Switch, Select, Dropdown, TabView, Dialog, MessageBox, NavMenu, Table, ProgressBar, LoadingSpinner, Pagination, CollapsePanel, Upload, FileBrowser, Avatar, Toast, Tooltip, Link, Border, Divider, Spacer, Form, InputNumber, GridLayout, FlexLayout, ResponsiveLayout, LayoutContainer
- **GPU-accelerated rendering**: batched quad renderer, SDF anti-aliased rounded rects, gradient support
- **Theme system**: light/dark themes with token-based design
- **CSS-like stylesheet** parser (type, #id, .class selectors)
- **Animation system**: fade, slide, opacity with easing functions
- **Z-index layering**: subtree-aware z-ordering for popups and overlays
- **Auto-scroll containers**: browser-style overflow detection with scrollbar
- **Frosted glass** blur effects
- **Icon system**: 24 built-in procedural icons
- **DPI-aware** font rendering (stb_truetype with .ttf/.ttc support)
- **Async utilities**: HTTP client, navigation, async task queue
- **Responsive layout**: percentage-based margin/spacing scaling

## Quick Start

```bash
# Build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build .

# Run
cp ../external/glfw/lib-vc2022/glfw3.dll Debug/
./Debug/exp.exe
```

## Architecture

```
src/core/       — App lifecycle, Window, Events, Painter, QuadBatcher, Animation
src/res/        — Shader, Texture, Font, Theme, StyleSheet, BlurShader
src/widgets/    — All UI widgets (50+ components)
src/utils/      — HttpClient, Navigation, AsyncTask
site/           — Documentation website
```

See [site/](site/) for full documentation and component demos.

## License

MIT
