#pragma once

#include "esphome.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/number/number.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include <cmath>

namespace esphome {
namespace s1 {

class LD2450 : public Component, public uart::UARTDevice {
 public:
  LD2450();

  void set_detection_range(number::Number *range) { detection_range = range; }
  void set_flip_y(switch_::Switch *sw) { flip_y = sw; }
  void set_exclusion_x_begin(number::Number *n) { exclusion_x_begin = n; }
  void set_exclusion_x_end(number::Number *n) { exclusion_x_end = n; }
  void set_exclusion_y_begin(number::Number *n) { exclusion_y_begin = n; }
  void set_exclusion_y_end(number::Number *n) { exclusion_y_end = n; }
  void set_tracking_mode_sensor(text_sensor::TextSensor *ts) { tracking_mode_ = ts; }

  void set_single_target_tracking();
  void set_multi_target_tracking();
  void restart_module();
  void restore_factory_settings();

  void setup() override;
  void loop() override;

 protected:
  void handle_ack_data(const uint8_t *buffer, int len);
  void parse_frame(const uint8_t *b);

 public:
  sensor::Sensor *target1_x, *target1_y, *target1_angle, *target1_speed, *target1_distance;
  sensor::Sensor *target2_x, *target2_y, *target2_angle, *target2_speed, *target2_distance;
  sensor::Sensor *target3_x, *target3_y, *target3_angle, *target3_speed, *target3_distance;
  number::Number *detection_range;
  switch_::Switch *flip_y;
  number::Number *exclusion_x_begin, *exclusion_x_end;
  number::Number *exclusion_y_begin, *exclusion_y_end;
  text_sensor::TextSensor *tracking_mode_;

 private:
  void send_command(const uint8_t *command, size_t length);
  uint16_t twoByteToUint(uint8_t firstByte, uint8_t secondByte);
};

inline LD2450::LD2450()
    : Component(), uart::UARTDevice(),
      detection_range(nullptr), flip_y(nullptr),
      exclusion_x_begin(nullptr), exclusion_x_end(nullptr),
      exclusion_y_begin(nullptr), exclusion_y_end(nullptr),
      tracking_mode_(nullptr) {
  target1_x        = new sensor::Sensor();
  target1_y        = new sensor::Sensor();
  target1_angle    = new sensor::Sensor();
  target1_speed    = new sensor::Sensor();
  target1_distance = new sensor::Sensor();

  target2_x        = new sensor::Sensor();
  target2_y        = new sensor::Sensor();
  target2_angle    = new sensor::Sensor();
  target2_speed    = new sensor::Sensor();
  target2_distance = new sensor::Sensor();

  target3_x        = new sensor::Sensor();
  target3_y        = new sensor::Sensor();
  target3_angle    = new sensor::Sensor();
  target3_speed    = new sensor::Sensor();
  target3_distance = new sensor::Sensor();
  tracking_mode_   = new text_sensor::TextSensor();
}

inline void LD2450::set_single_target_tracking() {
  static const uint8_t CMD[] = {0x02,0x00,0x80,0x00,0x04,0x03,0x02,0x01};
  send_command(CMD, sizeof(CMD));
}

inline void LD2450::set_multi_target_tracking() {
  static const uint8_t CMD[] = {0x02,0x00,0x90,0x00,0x04,0x03,0x02,0x01};
  send_command(CMD, sizeof(CMD));
}

inline void LD2450::restart_module() {
  static const uint8_t CMD[] = {0x02,0x00,0xA3,0x00,0x04,0x03,0x02,0x01};
  send_command(CMD, sizeof(CMD));
}

inline void LD2450::restore_factory_settings() {
  static const uint8_t CMD[] = {0x02,0x00,0xA2,0x00,0x04,0x03,0x02,0x01};
  send_command(CMD, sizeof(CMD));
}

inline void LD2450::setup() {
  if (tracking_mode_) tracking_mode_->publish_state("Multi");
  set_multi_target_tracking();
}

inline void LD2450::loop() {
  static const int MAX = 160;
  static uint8_t buf[MAX];
  static int pos = 0;

  while (available()) {
    int c = read();
    if (c < 0) break;
    if (pos < MAX) buf[pos++] = static_cast<uint8_t>(c);
    else pos = 0;

    if (pos >= 10 && buf[pos-4]==0x04 && buf[pos-3]==0x03 && buf[pos-2]==0x02 && buf[pos-1]==0x01) {
      handle_ack_data(buf, pos);
      pos = 0;
      continue;
    }

    if (pos >= 30 && buf[0]==0xAA && buf[1]==0xFF && buf[2]==0x03 && buf[3]==0x00 && buf[28]==0x55 && buf[29]==0xCC) {
      parse_frame(buf);
      pos = 0;
    }
  }
}

inline void LD2450::handle_ack_data(const uint8_t *buffer, int len) {
  if (len < 10) return;
  if (buffer[0]!=0xFD||buffer[1]!=0xFC||buffer[2]!=0xFB||buffer[3]!=0xFA) return;
  if (buffer[7]!=0x01) return;
  if (twoByteToUint(buffer[8],buffer[9])!=0x00) return;

  uint8_t cmd = buffer[6];
  if (tracking_mode_) {
    if (cmd == 0x80) {
      tracking_mode_->publish_state("Single");
    } else if (cmd == 0x90) {
      tracking_mode_->publish_state("Multi");
    }
  }
}

inline void LD2450::parse_frame(const uint8_t *b) {
  sensor::Sensor* xs[3] = {target1_x,target2_x,target3_x};
  sensor::Sensor* ys[3] = {target1_y,target2_y,target3_y};
  sensor::Sensor* angs[3] = {target1_angle,target2_angle,target3_angle};
  sensor::Sensor* sps[3] = {target1_speed,target2_speed,target3_speed};
  sensor::Sensor* dists[3] = {target1_distance,target2_distance,target3_distance};

  float range_cm=detection_range?detection_range->state:600.0f;
  bool do_flip=flip_y?flip_y->state:false;
  float xb=exclusion_x_begin?exclusion_x_begin->state:0;
  float xe=exclusion_x_end?exclusion_x_end->state:0;
  float yb=exclusion_y_begin?exclusion_y_begin->state:0;
  float ye=exclusion_y_end?exclusion_y_end->state:0;

  for(int t=0;t<3;++t){int base=4+t*8;auto raw16=[&](int off){return uint16_t(b[base+off])|(uint16_t(b[base+off+1])<<8);};auto to_s=[&](uint16_t v){return (v&0x8000)?int16_t(v&0x7FFF):-int16_t(v&0x7FFF);};float x_cm=float(to_s(raw16(0)))/10.0f;float y_cm=float(to_s(raw16(2)))/10.0f;float speed=float(to_s(raw16(4)));float dist=std::sqrt(x_cm*x_cm+y_cm*y_cm);float angle=(dist>0.0f)?(std::atan2(x_cm,y_cm)*180.0f/M_PI):0.0f;if(do_flip){x_cm=-x_cm;angle=-angle;}bool in_exclusion=(x_cm>=std::min(xb,xe))&&(x_cm<=std::max(xb,xe))&&(y_cm>=std::min(yb,ye))&&(y_cm<=std::max(yb,ye));if(dist<=range_cm&&!in_exclusion){xs[t]->publish_state(x_cm);ys[t]->publish_state(y_cm);angs[t]->publish_state(angle);sps[t]->publish_state(speed);dists[t]->publish_state(dist);}else{xs[t]->publish_state(0.0f);ys[t]->publish_state(0.0f);angs[t]->publish_state(0.0f);sps[t]->publish_state(0.0f);dists[t]->publish_state(0.0f);} }
}

inline void LD2450::send_command(const uint8_t *command,size_t length){static const uint8_t enable_cmd[]={0xFD,0xFC,0xFB,0xFA,0x04,0x00,0xFF,0x00,0x01,0x00,0x04,0x03,0x02,0x01};write_array(enable_cmd,sizeof(enable_cmd));static const uint8_t header[]={0xFD,0xFC,0xFB,0xFA};uint8_t full_cmd[sizeof(header)+length];memcpy(full_cmd,header,sizeof(header));memcpy(full_cmd+sizeof(header),command,length);write_array(full_cmd,sizeof(header)+length);static const uint8_t disable_cmd[]={0xFD,0xFC,0xFB,0xFA,0x02,0x00,0xFE,0x00,0x04,0x03,0x02,0x01};write_array(disable_cmd,sizeof(disable_cmd));}

inline uint16_t LD2450::twoByteToUint(uint8_t firstByte,uint8_t secondByte){return((uint16_t)secondByte<<8)|firstByte;}

}
}