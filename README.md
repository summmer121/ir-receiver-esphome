# ESPHome IR Receiver / ESPHome 红外接收器

[English](#english) | [中文](#chinese)

---

<a name="english"></a>
## English

ESP8266/ESP-12E based IR/RF receiver for Home Assistant.


https://github.com/summmer121/ir-receiver-esphome/blob/main/img/ha%E8%87%AA%E5%8A%A8%E5%8C%96.jpg
https://github.com/summmer121/ir-receiver-esphome/blob/main/img/web%E7%95%8C%E9%9D%A2.jpg
### Features

- Receives **RCSwitch raw codes** on D2/GPIO4
- Learns key/value mappings from ESPHome web/API controls
- Persists mappings to ESP8266 SPIFFS at `/keymap.txt` — **survives reboots**
- MQTT configuration via web controls, restored from flash on boot
- Publishes matched events to MQTT:
  - `ir_receiver/key` — raw received key
  - `ir_receiver/value` — mapped value
- Adds **press_counter** text sensor for reliable Home Assistant automation triggers (increments on every matched key press)
- Auto-fills received IR code into the **ircode** text box (no manual fill button needed)
  - If matched to an existing mapping, the **value** box is also auto-filled with the corresponding value
  - If unmatched, the **value** box shows `unknow` — just enter a new value and save
- Clean MQTT: only publishes `ir_receiver/key` and `ir_receiver/value`

### Hardware

| Component | Pin |
|-----------|-----|
| Development Board | **ESP8266 ESP-12E** (NodeMCU compatible) |
| IR/RF Receiver Data | **D2 = GPIO4** |
| Power | 3.3V / USB |

### Files

| File | Description |
|------|-------------|
| `ir-receiver.yaml` | ESPHome node configuration |
| `ir_keymap.h` | C++ helper for mapping persistence and lookup |

### Quick Start

1. Copy both files to your ESPHome config directory.
2. Add secrets in `secrets.yaml`:

```yaml
wifi_ssid: "your_wifi"
wifi_password: "your_password"
api_encryption_key: "your_api_key"
ota_password: "your_ota_password"
```

3. Compile and upload with ESPHome.
4. Access the device web UI or Home Assistant to:
   - **Press any remote button** — both IR code and value automatically appear in their respective boxes
   - If value shows `unknow`, enter a **Value** (e.g., `off`, `time`, `channel_1`)
   - Press **Save Mapping** to persist it to SPIFFS
   - Enter MQTT broker settings and reboot to enable MQTT forwarding

### MQTT Payloads

When a received code has a saved value:

| Topic | Example Payload |
|-------|-----------------|
| `ir_receiver/key` | `11011000000` |
| `ir_receiver/value` | `off` |

Use these topics in **Home Assistant automations**.

### Web UI Controls

| Control | Function |
|---------|----------|
| IR Code (text) | Auto-fills with received IR code / manual input |
| Value (text) | Mapped value to save (auto-fills if matched) |
| Save Mapping | Save key/value to persistent storage (both required) |
| Delete Mapping | Remove by **key** or **value** — enter either |
| List All | Show all saved mappings |
| Last Value | Display the **value** of the most recently matched key (for automation triggers) |
| Press Counter | Increments on every matched key press (for HA automation triggers) |
| MQTT Server / Port / Username / Password | Configure MQTT broker |
| Reboot Device | Restart to apply MQTT changes |

### Firmware (Pre-built Binary)

Download the latest OTA binary from [Releases](https://github.com/summmer121/ir-receiver-esphome/releases).

Flash using ESPHome web dashboard or `esphome upload`.

---

<a name="chinese"></a>
## 中文

基于 ESP8266/ESP-12E 的红外/射频接收器，接入 Home Assistant。

https://github.com/summmer121/ir-receiver-esphome/blob/main/img/ha%E8%87%AA%E5%8A%A8%E5%8C%96.jpg
https://github.com/summmer121/ir-receiver-esphome/blob/main/img/web%E7%95%8C%E9%9D%A2.jpg
### 功能特性

- 在 **D2/GPIO4** 接收 **RCSwitch Raw** 信号
- 通过 ESPHome 网页/API 学习键值映射
- 映射保存到 ESP8266 SPIFFS `/keymap.txt` — **重启不丢失**
- MQTT 配置通过网页输入，重启后从 Flash 自动恢复
- 仅当 key 匹配到 value 时向 MQTT 发送：
  - `ir_receiver/key` — 原始红外码
  - `ir_receiver/value` — 映射值
- 新增 **按键计数 presscount** 传感器，每次匹配成功自动 +1，用于 Home Assistant 自动化触发
- **红外码自动填入**：收到红外信号后自动填入“红外码”文本框，无需手动点击
  - 匹配到已有映射时，“键值”框同时自动填入对应 value
  - 未匹配时，“键值”框显示 `unknow`，直接输入新值后保存即可
- **未匹配的 key 不发送 MQTT**，避免垃圾消息

### 硬件信息

| 组件 | 引脚 |
|------|------|
| 开发板 | **ESP8266 ESP-12E**（NodeMCU 兼容） |
| 红外/射频接收数据脚 | **D2 = GPIO4** |
| 供电 | 3.3V / USB |

### 文件说明

| 文件 | 说明 |
|------|------|
| `ir-receiver.yaml` | ESPHome 主配置文件 |
| `ir_keymap.h` | C++ 映射持久化和查找库 |

### 快速开始

1. 将两个文件放到 ESPHome 配置目录。
2. 在 `secrets.yaml` 中添加密钥：

```yaml
wifi_ssid: "你的WiFi"
wifi_password: "WiFi密码"
api_encryption_key: "API密钥"
ota_password: "OTA密码"
```

3. 用 ESPHome 编译并上传。
4. 打开设备网页或 Home Assistant：
   - **按遥控器任意键** — 红外码和键值自动同时填入两个框中
   - 如果键值显示 `unknow`，输入 **键值**（如 `off`、`time`、`channel_1`）
   - 点 **保存映射** 写入持久存储
   - 输入 MQTT 服务器地址、端口、用户名、密码后 **重启设备** 生效

### MQTT 数据格式

当接收到的红外码已保存映射时：

| 主题 | 示例内容 |
|------|----------|
| `ir_receiver/key` | `11011000000` |
| `ir_receiver/value` | `off` |

在 **Home Assistant 自动化** 中监听这两个主题即可触发动作。

### 网页控制说明

| 控件 | 功能 |
|------|------|
| 红外码 ircode | 自动填入接收的红外码 / 手动输入 |
| 键值 value | 映射值（匹配时自动填入） |
| 保存映射 save | 必须红外码和键值都不为空才能保存 |
| 删除映射 delete | 输入 **key** 或 **value** 任意一个匹配即删除 |
| 列出所有映射 listall | 显示所有已保存映射 |
| 最近值 lastvalue | 显示最近一次匹配到的 **value**（方便自动化触发） |
| 按键计数 presscount | 每次匹配成功自动 +1（用于 HA 自动化触发） |
| MQTT服务器/端口/用户名/密码 | 配置 MQTT 转发参数 |
| 重启设备 reboot | 重启使 MQTT 配置生效 |

### 固件（预编译二进制）

从 [Releases](https://github.com/summmer121/ir-receiver-esphome/releases) 下载最新 OTA 固件。

通过 ESPHome 网页面板或 `esphome upload` 刷入。

---

## License

MIT
