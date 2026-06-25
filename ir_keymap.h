#pragma once
#include <map>
#include <string>
#include <vector>
#include <FS.h>
#include "esphome/components/mqtt/mqtt_client.h"

namespace ir_keymap {
  static std::map<std::string, std::string> cache;
  static const char* KEYMAP_FILE = "/keymap.txt";
  static bool fs_ready = false;

  std::string trim(const std::string& value) {
    size_t start = value.find_first_not_of(" \t\r\n'\"");
    if (start == std::string::npos) return "";
    size_t end = value.find_last_not_of(" \t\r\n'\"");
    return value.substr(start, end - start + 1);
  }

  bool load_line(const std::string& raw_line) {
    std::string line = trim(raw_line);
    if (line.empty()) return false;
    size_t sep = line.find('=');
    if (sep == std::string::npos || sep == 0) return false;
    std::string key = trim(line.substr(0, sep));
    std::string val = trim(line.substr(sep + 1));
    if (key.empty() || val.empty()) return false;
    cache[key] = val;
    return true;
  }

  std::string to_blob() {
    std::string out;
    for (auto& kv : cache) {
      out += kv.first;
      out += "=";
      out += kv.second;
      out += "\n";
    }
    return out;
  }

  void load_blob(const std::string& blob) {
    cache.clear();
    size_t start = 0;
    while (start < blob.size()) {
      size_t end = blob.find('\n', start);
      if (end == std::string::npos) end = blob.size();
      load_line(blob.substr(start, end - start));
      start = end + 1;
    }
  }

  bool begin_fs() {
    if (fs_ready) return true;
    fs_ready = SPIFFS.begin();
    if (!fs_ready) {
      ESP_LOGW("keymap", "SPIFFS mount failed, formatting filesystem once...");
      SPIFFS.format();
      fs_ready = SPIFFS.begin();
    }
    ESP_LOGI("keymap", "SPIFFS mount %s", fs_ready ? "OK" : "FAILED");
    return fs_ready;
  }

  bool load_fs() {
    cache.clear();
    if (!begin_fs()) return false;
    if (!SPIFFS.exists(KEYMAP_FILE)) {
      ESP_LOGI("keymap", "No %s yet, start with empty keymap", KEYMAP_FILE);
      return true;
    }
    File f = SPIFFS.open(KEYMAP_FILE, "r");
    if (!f) {
      ESP_LOGE("keymap", "Failed to open %s for reading", KEYMAP_FILE);
      return false;
    }
    std::string data;
    while (f.available()) data += (char) f.read();
    f.close();
    load_blob(data);
    ESP_LOGI("keymap", "Loaded %u mappings from SPIFFS, %u bytes", (unsigned) cache.size(), (unsigned) data.size());
    return true;
  }

  bool save_fs() {
    if (!begin_fs()) return false;
    std::string data = to_blob();
    File f = SPIFFS.open(KEYMAP_FILE, "w");
    if (!f) {
      ESP_LOGE("keymap", "Failed to open %s for writing", KEYMAP_FILE);
      return false;
    }
    size_t written = f.print(data.c_str());
    f.flush();
    f.close();
    bool ok = written == data.size();
    ESP_LOGI("keymap", "SPIFFS save %s: %u mappings, %u/%u bytes", ok ? "OK" : "FAILED", (unsigned) cache.size(), (unsigned) written, (unsigned) data.size());
    return ok;
  }

  bool migrate_from_blob_if_needed(const std::string& old_blob) {
    if (!cache.empty()) return false;
    std::string blob = trim(old_blob);
    if (blob.empty()) return false;
    load_blob(blob);
    if (cache.empty()) return false;
    bool ok = save_fs();
    ESP_LOGI("keymap", "Migrated old preference keymap to SPIFFS: %s", ok ? "OK" : "FAILED");
    return ok;
  }

  std::string uint64_to_bin(uint64_t value) {
    if (value == 0) return "0";
    std::string out;
    bool started = false;
    for (int i = 63; i >= 0; i--) {
      bool bit = (value >> i) & 1ULL;
      if (bit) started = true;
      if (started) out += bit ? '1' : '0';
    }
    return out;
  }

  std::string get(const std::string& raw_code) {
    std::string code = trim(raw_code);
    auto it = cache.find(code);
    if (it != cache.end()) return it->second;

    const std::string prefix = "RCSWITCH:";
    if (code.rfind(prefix, 0) == 0) {
      size_t pos = code.find(':', prefix.size());
      if (pos != std::string::npos) {
        std::string bare = code.substr(pos + 1);
        it = cache.find(bare);
        if (it != cache.end()) return it->second;
      }
    }
    return "";
  }

  std::string find_key_by_value(const std::string& raw_value) {
    std::string value = trim(raw_value);
    for (auto& kv : cache) {
      if (kv.second == value) return kv.first;
    }
    return "";
  }

  bool set_cache(const std::string& raw_code, const std::string& raw_value) {
    std::string code = trim(raw_code);
    std::string value = trim(raw_value);
    if (code.empty() || value.empty()) return false;
    cache[code] = value;
    return true;
  }

  bool set(const std::string& raw_code, const std::string& raw_value) {
    if (!set_cache(raw_code, raw_value)) return false;
    return save_fs();
  }

  bool remove_cache(const std::string& raw_code) {
    std::string code = trim(raw_code);
    auto it = cache.find(code);
    if (it != cache.end()) {
      cache.erase(it);
      return true;
    }
    return false;
  }

  bool remove(const std::string& raw_code) {
    bool removed = remove_cache(raw_code);
    if (removed) save_fs();
    return removed;
  }

  size_t size() { return cache.size(); }

  std::string summary(size_t max_items = 8) {
    if (cache.empty()) return "（暂无映射）";
    std::string out;
    size_t i = 0;
    for (auto& kv : cache) {
      if (i >= max_items) break;
      if (!out.empty()) out += " | ";
      out += kv.first + " → " + kv.second;
      i++;
    }
    if (cache.size() > max_items) out += " | ... 共" + std::to_string(cache.size()) + "条";
    return out;
  }

  void log_all() {
    if (cache.empty()) {
      ESP_LOGI("keymap", "List all: empty");
      return;
    }
    ESP_LOGI("keymap", "List all: %u mappings", (unsigned) cache.size());
    for (auto& kv : cache) ESP_LOGI("keymap", "PAIR key=%s value=%s", kv.first.c_str(), kv.second.c_str());
  }
}
