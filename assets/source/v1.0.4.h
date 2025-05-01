#include "esphome.h"
#include <cmath>
#include <vector>
#include <cstring>

using namespace esphome;

class LD2450 : public Component, public UARTDevice {
 public:
  LD2450(UARTComponent *parent) : UARTDevice(parent) {
    fw_version = new TextSensor();
    mac_address = new TextSensor();
    tracking_mode = new TextSensor();

    target1_x = new sensor::Sensor();
    target1_y = new sensor::Sensor();
    target1_angle = new sensor::Sensor();
    target1_speed = new sensor::Sensor();
    target1_distance_resolution = new sensor::Sensor();
    target1_distance = new sensor::Sensor();

    target2_x = new sensor::Sensor();
    target2_y = new sensor::Sensor();
    target2_angle = new sensor::Sensor();
    target2_speed = new sensor::Sensor();
    target2_distance_resolution = new sensor::Sensor();
    target2_distance = new sensor::Sensor();

    target3_x = new sensor::Sensor();
    target3_y = new sensor::Sensor();
    target3_angle = new sensor::Sensor();
    target3_speed = new sensor::Sensor();
    target3_distance_resolution = new sensor::Sensor();
    target3_distance = new sensor::Sensor();

    detection_range_threshold = 600.0;
  }

  void setup() override {}

  void set_single_target_tracking() {
    static const uint8_t command[] = {0x02, 0x00, 0x80, 0x00, 0x04, 0x03, 0x02, 0x01};
    send_command(command, sizeof(command));
  }

  void set_multi_target_tracking() {
    static const uint8_t command[] = {0x02, 0x00, 0x90, 0x00, 0x04, 0x03, 0x02, 0x01};
    send_command(command, sizeof(command));
  }

  void turn_bluetooth_on() {
    static const uint8_t command[] = {0x04, 0x00, 0xA4, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
    send_command(command, sizeof(command));
  }

  void turn_bluetooth_off() {
    static const uint8_t command[] = {0x04, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
    send_command(command, sizeof(command));
  }

  void restart_module() {
    static const uint8_t command[] = {0x02, 0x00, 0xA3, 0x00, 0x04, 0x03, 0x02, 0x01};
    send_command(command, sizeof(command));
  }

  void restore_factory_settings() {
    static const uint8_t command[] = {0x02, 0x00, 0xA2, 0x00, 0x04, 0x03, 0x02, 0x01};
    send_command(command, sizeof(command));
  }

  void read_firmware() {
    static const uint8_t command[] = {0x02, 0x00, 0xA0, 0x00, 0x04, 0x03, 0x02, 0x01};
    send_command(command, sizeof(command));
  }

  void read_mac_address() {
    static const uint8_t command[] = {0x04, 0x00, 0xA5, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
    send_command(command, sizeof(command));
  }

  void set_detection_range_threshold(float range_cm) {
    detection_range_threshold = range_cm;
  }

  void loop() override {
    static const int max_line_length = 160;
    static uint8_t buffer[max_line_length];
    static int pos = 0;
    while (available()) {
      int c = read();
      if (c < 0) break;
      if (pos < max_line_length - 1) {
        buffer[pos++] = c;
      } else {
        pos = 0;
      }
      if (pos >= 10) {
        if (buffer[pos-4] == 0x04 && buffer[pos-3] == 0x03 &&
            buffer[pos-2] == 0x02 && buffer[pos-1] == 0x01) {
          handle_ack_data(buffer, pos);
          pos = 0;
          awaiting_response = false;
          continue;
        }
      }
      if (pos >= 30) {
        if (buffer[0] == 0xAA && buffer[1] == 0xFF && buffer[2] == 0x03 &&
            buffer[3] == 0x00 && buffer[28] == 0x55 && buffer[29] == 0xCC) {
          handle_radar_data(buffer, 30);
          pos = 0;
          continue;
        }
      }
    }
  }

  std::vector<TextSensor*> get_text_sensors() {
    std::vector<TextSensor*> sensors;
    sensors.push_back(fw_version);
    sensors.push_back(mac_address);
    sensors.push_back(tracking_mode);
    return sensors;
  }

  std::vector<sensor::Sensor*> get_target_sensors() {
    std::vector<sensor::Sensor*> sensors;
    sensors.push_back(target1_x);
    sensors.push_back(target1_y);
    sensors.push_back(target1_angle);
    sensors.push_back(target1_speed);
    sensors.push_back(target1_distance_resolution);
    sensors.push_back(target1_distance);
    sensors.push_back(target2_x);
    sensors.push_back(target2_y);
    sensors.push_back(target2_angle);
    sensors.push_back(target2_speed);
    sensors.push_back(target2_distance_resolution);
    sensors.push_back(target2_distance);
    sensors.push_back(target3_x);
    sensors.push_back(target3_y);
    sensors.push_back(target3_angle);
    sensors.push_back(target3_speed);
    sensors.push_back(target3_distance_resolution);
    sensors.push_back(target3_distance);
    return sensors;
  }

  TextSensor *fw_version;
  TextSensor *mac_address;
  TextSensor *tracking_mode;

  sensor::Sensor *target1_x;
  sensor::Sensor *target1_y;
  sensor::Sensor *target1_angle;
  sensor::Sensor *target1_speed;
  sensor::Sensor *target1_distance_resolution;
  sensor::Sensor *target1_distance;

  sensor::Sensor *target2_x;
  sensor::Sensor *target2_y;
  sensor::Sensor *target2_angle;
  sensor::Sensor *target2_speed;
  sensor::Sensor *target2_distance_resolution;
  sensor::Sensor *target2_distance;

  sensor::Sensor *target3_x;
  sensor::Sensor *target3_y;
  sensor::Sensor *target3_angle;
  sensor::Sensor *target3_speed;
  sensor::Sensor *target3_distance_resolution;
  sensor::Sensor *target3_distance;

  float detection_range_threshold;

 private:
  void send_command(const uint8_t *command, size_t length) {
    static const uint8_t enable_cmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
    write_array(enable_cmd, sizeof(enable_cmd));
    static const uint8_t header[] = {0xFD, 0xFC, 0xFB, 0xFA};
    uint8_t full_command[sizeof(header) + length];
    memcpy(full_command, header, sizeof(header));
    memcpy(full_command + sizeof(header), command, length);
    write_array(full_command, sizeof(full_command));
    static const uint8_t disable_cmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00, 0x04, 0x03, 0x02, 0x01};
    write_array(disable_cmd, sizeof(disable_cmd));
    awaiting_response = true;
  }

  void handle_ack_data(const uint8_t *buffer, int len) {
    if (len < 10) return;
    if (buffer[0] != 0xFD || buffer[1] != 0xFC ||
        buffer[2] != 0xFB || buffer[3] != 0xFA)
      return;
    if (buffer[7] != 0x01) return;
    uint16_t result = twoByteToUint(buffer[8], buffer[9]);
    if (result != 0x00) return;
    uint8_t cmd = buffer[6];
    if (cmd == 0x80) {
      tracking_mode->publish_state("Single");
    } else if (cmd == 0x90) {
      tracking_mode->publish_state("Multi");
    } else if (cmd == 0xA0) {
      char version[18];
      snprintf(version, sizeof(version), "v%01X.%02X.%02X%02X%02X%02X",
               buffer[13], buffer[12], buffer[17], buffer[16],
               buffer[15], buffer[14]);
      fw_version->publish_state(version);
    } else if (cmd == 0xA5) {
      char mac[18];
      snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
               buffer[10], buffer[11], buffer[12], buffer[13],
               buffer[14], buffer[15]);
      mac_address->publish_state(mac);
    }
  }

  void handle_radar_data(const uint8_t *buffer, int len) {
    for (int target = 0; target < 3; target++) {
      int base = 4 + target * 8;
      uint16_t x_raw = (uint16_t)buffer[base] | ((uint16_t)buffer[base + 1] << 8);
      int16_t x_val = (x_raw < 32768) ? -((int16_t)x_raw) : (int16_t)(x_raw - 32768);
      uint16_t y_raw = (uint16_t)buffer[base + 2] | ((uint16_t)buffer[base + 3] << 8);
      int16_t y_val = (y_raw < 32768) ? -((int16_t)y_raw) : (int16_t)(y_raw - 32768);
      uint16_t speed_raw = (uint16_t)buffer[base + 4] | ((uint16_t)buffer[base + 5] << 8);
      int16_t speed_val = (speed_raw < 32768) ? -((int16_t)speed_raw) : (int16_t)(speed_raw - 32768);
      uint16_t resolution = (uint16_t)buffer[base + 6] | ((uint16_t)buffer[base + 7] << 8);
      float x_cm = ((float)x_val) / 10.0;
      float y_cm = ((float)y_val) / 10.0;
      int dist_res_cm = resolution / 10;
      float calc_distance = sqrt(x_cm * x_cm + y_cm * y_cm);
      float angle = (calc_distance > 0) ? (atan2(x_cm, y_cm) * 180.0 / M_PI) : 0.0;


      if (calc_distance > detection_range_threshold) {
        if (target == 0) {
          target1_x->publish_state(0);
          target1_y->publish_state(0);
          target1_angle->publish_state(0);
          target1_speed->publish_state(0);
          target1_distance_resolution->publish_state(0);
          target1_distance->publish_state(0);
        } else if (target == 1) {
          target2_x->publish_state(0);
          target2_y->publish_state(0);
          target2_angle->publish_state(0);
          target2_speed->publish_state(0);
          target2_distance_resolution->publish_state(0);
          target2_distance->publish_state(0);
        } else if (target == 2) {
          target3_x->publish_state(0);
          target3_y->publish_state(0);
          target3_angle->publish_state(0);
          target3_speed->publish_state(0);
          target3_distance_resolution->publish_state(0);
          target3_distance->publish_state(0);
        }
        continue;
      }

      if (target == 0) {
        target1_x->publish_state(x_cm);
        target1_y->publish_state(y_cm);
        target1_angle->publish_state(angle);
        target1_speed->publish_state(speed_val);
        target1_distance_resolution->publish_state(dist_res_cm);
        target1_distance->publish_state(calc_distance);
        ESP_LOGD("ld2450", "Target 1: X=%.2f cm, Y=%.2f cm", x_cm, y_cm);
      } else if (target == 1) {
        target2_x->publish_state(x_cm);
        target2_y->publish_state(y_cm);
        target2_angle->publish_state(angle);
        target2_speed->publish_state(speed_val);
        target2_distance_resolution->publish_state(dist_res_cm);
        target2_distance->publish_state(calc_distance);
      } else if (target == 2) {
        target3_x->publish_state(x_cm);
        target3_y->publish_state(y_cm);
        target3_angle->publish_state(angle);
        target3_speed->publish_state(speed_val);
        target3_distance_resolution->publish_state(dist_res_cm);
        target3_distance->publish_state(calc_distance);
      }
    }
  }

  uint16_t twoByteToUint(uint8_t firstByte, uint8_t secondByte) {
    return ((uint16_t)secondByte << 8) | firstByte;
  }

  bool awaiting_response = false;
};