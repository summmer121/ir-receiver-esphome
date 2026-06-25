# ESPHome IR Receiver

ESP8266/ESP-12E based IR/RF receiver for Home Assistant.

Features:

- Receives RCSwitch raw codes on D2/GPIO4.
- Learns key/value mappings from ESPHome web/API controls.
- Persists mappings to ESP8266 SPIFFS at `/keymap.txt` so reboot does not lose mappings.
- MQTT configuration is entered from web controls and restored from flash.
- Publishes only matched events to MQTT:
  - `ir_receiver/key`
  - `ir_receiver/value`
- If a received key has no mapped value, MQTT is not published.

## Files

- `ir-receiver.yaml` — ESPHome node configuration.
- `ir_keymap.h` — C++ helper for mapping persistence and lookup.

## Usage

1. Put both files in your ESPHome config directory.
2. Add secrets in `secrets.yaml`:

```yaml
wifi_ssid: your_wifi
wifi_password: your_password
api_encryption_key: your_api_key
ota_password: your_ota_password
```

3. Compile/upload with ESPHome.
4. Use the web/API controls to:
   - Fill latest received code.
   - Enter value.
   - Save mapping.
   - Configure MQTT broker/port/user/password and reboot.

## MQTT payloads

When a received code has a saved value:

```text
ir_receiver/key    -> raw key, e.g. 11011000000
ir_receiver/value  -> mapped value, e.g. off
```

These topics are intended for Home Assistant automations.
