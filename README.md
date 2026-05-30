# ESP32 实战系列 — 配套代码

[ESP32 核心基础：芯片架构、FreeRTOS 与 GPIO 编程](https://blog.syouyu.com) 文章的配套代码仓库。

## 硬件要求

- **开发板**: NodeMCU-32S 或 ESP32-DevKitC（经典 ESP32，Xtensa LX6 双核）
- **外设**: LED ×1、220Ω 电阻 ×1、按钮 ×1、面包板、杜邦线若干
- **环境**: [PlatformIO](https://platformio.org/) (VS Code 插件) + Arduino 框架

## 接线图

```text
        ESP32
     ┌─────────┐
     │    GND  │──────────┬──────────── 面包板 GND 轨
     │         │          │
     │   GPIO5 │──────────├──[220Ω]──→ LED → GND
     │         │          │
     │   GPIO0 │──────────┤├─────────── 按钮 ──── GND
     │         │          │
     │   3.3V  │──────────┴──────────── 面包板 VCC 轨
     └─────────┘

LED：阳极（长脚）→ 220Ω 限流电阻 → GPIO5，阴极 → GND
按钮：一端 → GPIO0（内部上拉），另一端 → GND
```

> GPIO0 在经典 ESP32 上通常用作 Boot 按钮。直接使用板载 Boot 按钮无需额外接线。

## 示例说明

三个示例通过 `platformio.ini` 中的 `build_flags` 条件编译：

```ini
build_flags = -D EXAMPLE=1   # 轮询
build_flags = -D EXAMPLE=2   # 中断
build_flags = -D EXAMPLE=3   # 中断 + FreeRTOS Queue
```

### 编译与烧录

| 示例 | 命令 |
|:-----|:-----|
| 轮询 | `pio run -e polling -t upload` |
| 中断 | `pio run -e interrupt -t upload` |
| 中断+Queue | `pio run -e interrupt_queue -t upload` |

烧录后运行串口监视器查看输出：

```bash
pio device monitor
```

按下按钮，串口应输出 `LED: ON` / `LED: OFF`。

## 三个写法的演进

| 写法 | 文件 | 特点 |
|:-----|:-----|:-----|
| 轮询 | `src/main.cpp` (`EXAMPLE=1`) | `loop()` 中不断检查按钮状态，最简单的实现 |
| 中断 | `src/main.cpp` (`EXAMPLE=2`) | `attachInterrupt` 硬件触发，ISR 去抖，volatile 标志位 |
| 中断+Queue | `src/main.cpp` (`EXAMPLE=3`) | ISR → `xQueueSendFromISR` → Task → `xQueueReceive`，FreeRTOS 解耦 |

三种写法共用相同硬件，逐级演进，体现嵌入式代码从"能跑"到"干净、高效、可扩展"的过程。

## 系列文章

1. **ESP32 核心基础：芯片架构、FreeRTOS 与 GPIO 编程** ← 当前
2. ESP32 WiFi 连接管理与 MQTT 协议（即将发布）
3. ESP32 传感器采集与数据管道（即将发布）
4. 生产级 ESP32：OTA 升级、低功耗与 BLE 配网（即将发布）

## License

MIT
