# YAN · 一场道别的旅途 (Farewell_Trip)

> Qt 6 / C++17 叙事驱动独立游戏  
> 机器人少女 YAN 从沉睡中苏醒，穿越五重幻境，奔赴一场盛大的道别。

---

## 目录结构

```
Farewell_Trip/
│
├── README.md                         # 本文件
├── CMakeLists.txt                    # CMake 构建脚本
│
├── src/                              # 【源代码】
│   ├── main.cpp                      # 程序入口
│   ├── core/                         # 引擎核心 (GameEngine, EventBus, Config, Input, Resource, Transition)
│   ├── ui/                           # UI 框架 (MainWindow, GameView, GameScene, HUD)
│   ├── entity/                       # 游戏实体 (Player, Enemy)
│   ├── combat/                       # 战斗系统 (CombatSystem)
│   ├── dialogue/                     # 对话系统 (DialogueManager)
│   ├── animation/                    # 动画系统 (SpriteAnimation)
│   ├── video/                        # 过场动画 (CutsceneManager)
│   ├── audio/                        # 【预留】音频系统
│   ├── utils/                        # 工具类 (MathUtils, FPSCounter)
│   ├── scene/                        # 章节调度 (SceneManager)
│   └── chapters/                     # 章节剧情
│       ├── ChapterBase.h             #   章节基类
│       ├── Chapter1_Awaken.*         #   第1章：少女！从沉睡中苏醒
│       ├── Chapter2_PeachBlossom.*   #   第2章：少女！决战梨花怪
│       ├── Chapter3_Rebellion.*      #   第3章：少女！向天空高举叛逆之拳
│       ├── Chapter4_Survivor.*       #   第4章：少女！最后的幸存者
│       └── Chapter5_Farewell.*       #   第5章：少女！银河告别旅行
│
├── resources/                        # 【美术资源】
│   ├── resources.qrc                 #   Qt 资源文件（注册所有资源）
│   ├── styles/
│   │   └── main.qss                  #   全局样式表
│   ├── images/
│   │   ├── common/                   #   通用/参考图片
│   │   ├── ch01_awaken/              #   第1章图片
│   │   ├── ch02_peach/               #   第2章图片
│   │   ├── ch03_rebellion/           #   第3章图片
│   │   ├── ch04_survivor/            #   第4章图片
│   │   └── ch05_farewell/            #   第5章图片
│   ├── videos/
│   │   ├── ch03_rebellion/           #   第3章视频
│   │   ├── ch04_survivor/            #   第4章视频
│   │   └── ch05_farewell/            #   第5章视频
│   └── music/                        #   【预留】音乐文件
│
└── build/                            # 构建输出（不纳入版本控制）
```

## 依赖

- **Qt 6.5+** (Core, Gui, Widgets, Multimedia, MultimediaWidgets)
- **CMake 3.19+**
- C++17 编译器 (MSVC 2019+ / MinGW GCC 11+)

## 构建

```bash
# 配置 (MinGW)
cmake -B build -G "MinGW Makefiles" \
  -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/mingw_64" \
  -DCMAKE_BUILD_TYPE=Debug

# 构建
cmake --build build

# 运行
./build/Farewell_Trip.exe
```

## 操作说明

| 按键 | 功能 |
|------|------|
| `A` / `D` 或 `←` / `→` | 左右移动 |
| `K` | 跳跃 |
| `J` | 攻击 |
| `空格` / 鼠标左键 | 推进对话 |
| `ESC` | 暂停 / 继续 |

## 章节概览

| 章节 | 标题 | 场景 | Boss |
|------|------|------|------|
| 第1章 | 少女！从沉睡中苏醒 | 废弃密室 | — |
| 第2章 | 少女！决战梨花怪 | 森林 → 十里桃林 | 桃子怪 |
| 第3章 | 少女！向天空高举叛逆之拳 | 纽约 → 上海 → 上政 | 旧日执念·前女友 |
| 第4章 | 少女！最后的幸存者 | 南京 → 悬空之桥 → 空白终境 | 幻境执念·渣男 |
| 第5章 | 少女！银河告别旅行 | 终章诗歌 | — |

## 开发备注

- 当前版本使用**纯色占位符**渲染角色和背景（参见 `GameConfig.h` 中的 PlaceholderColor）
- 美术资源已按章节整理到 `resources/` 下，后续可直接通过 QRC 别名引用
- 音频系统目录已预留 (`src/audio/` + `resources/music/`)
