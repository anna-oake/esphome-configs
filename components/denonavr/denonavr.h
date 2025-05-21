#pragma once

#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"
#include <iomanip>
#include <map>
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

namespace esphome {
namespace denonavr {

static const uint8_t MAX_ZONES = 3;
static const uint8_t MAX_LINE_LENGTH = 60; // Max characters for serial buffer
inline const std::vector<std::string> EVENTS = {
    "MVMAX", "STBY", "DIM", "ECO", "NSA", "NSE", "SLP", "CV", "DC",
    "HD",    "MN",   "MS",  "MU",  "MV",  "NS",  "OP",  "PS", "PV",
    "PW",    "RM",   "SD",  "SI",  "SR",  "SS",  "SV",  "SY", "TF",
    "TM",    "TP",   "TR",  "UG",  "VS",  "Z2",  "Z3",  "ZM"};
inline const std::vector<std::string> ALL_ZONE_EVENTS = {
    "DIM", "NSA", "NSE", "HD", "NS", "MN", "PW",
    "RM",  "SY",  "TF",  "TM", "TP", "TR", "UG"};
inline const std::vector<std::string> SOURCES = {
    "CD",         "PHONO",     "TUNER",  "DVD",     "BD",       "TV",
    "SAT/CBL",    "MPLAY",     "GAME",   "HDRADIO", "NET",      "PANDORA",
    "SIRIUSXM",   "SOURCE",    "LASTFM", "FLICKR",  "IRADIO",   "IRP",
    "SERVER",     "FAVORITES", "AUX1",   "AUX2",    "AUX3",     "AUX4",
    "AUX5",       "AUX6",      "AUX7",   "BT",      "USB/IPOD", "USB DIRECT",
    "IPOD DIRECT"};

enum ZoneStructure : uint8_t {
  ZONE_ALL = 0,
  ZONE_MAIN = 1,
  ZONE_2 = 2,
  ZONE_3 = 3,
};

enum Channel : uint8_t {
  CHANNEL_FRONT_LEFT = 1 << 0,
  CHANNEL_FRONT_RIGHT = 1 << 1,
  CHANNEL_CENTRE = 1 << 2,
  CHANNEL_SUBWOOFER = 1 << 3,
  CHANNEL_SURROUND_LEFT = 1 << 4,
  CHANNEL_SURROUND_RIGHT = 1 << 5,
  CHANNEL_REAR_LEFT = 1 << 6,
  CHANNEL_REAR_RIGHT = 1 << 7
};

enum ChannelConfig : uint8_t {
  CONFIG_2_0 = CHANNEL_FRONT_LEFT | CHANNEL_FRONT_RIGHT,
  CONFIG_2_1 = CONFIG_2_0 | CHANNEL_SUBWOOFER,

  CONFIG_3_0 = CONFIG_2_0 | CHANNEL_CENTRE,
  CONFIG_3_1 = CONFIG_3_0 | CHANNEL_SUBWOOFER,

  CONFIG_4_0 = CONFIG_2_0 | CHANNEL_SURROUND_LEFT | CHANNEL_SURROUND_RIGHT,
  CONFIG_4_1 = CONFIG_4_0 | CHANNEL_SUBWOOFER,

  CONFIG_5_0 = CONFIG_4_0 | CHANNEL_CENTRE,
  CONFIG_5_1 = CONFIG_5_0 | CHANNEL_SUBWOOFER,

  CONFIG_7_0 = CONFIG_5_0 | CHANNEL_REAR_LEFT | CHANNEL_REAR_RIGHT,
  CONFIG_7_1 = CONFIG_7_0 | CHANNEL_SUBWOOFER,
};

inline std::string channel_config_to_string(ChannelConfig cfg) {
  switch (cfg) {
  case CONFIG_2_0:
    return "2.0";
  case CONFIG_2_1:
    return "2.1";
  case CONFIG_3_0:
    return "3.0";
  case CONFIG_3_1:
    return "3.1";
  case CONFIG_4_0:
    return "4.0";
  case CONFIG_4_1:
    return "4.1";
  case CONFIG_5_0:
    return "5.0";
  case CONFIG_5_1:
    return "5.1";
  case CONFIG_7_0:
    return "7.0";
  case CONFIG_7_1:
    return "7.1";
  default:
    return "0.0";
  }
}

class DenonAVRComponent : public Component, public uart::UARTDevice {
#ifdef USE_SWITCH
  SUB_SWITCH(power)
#endif
#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(input_channels)
  SUB_TEXT_SENSOR(output_channels)
#endif

public:
  DenonAVRComponent();
  void setup() override;
  void dump_config() override;
  void loop() override;
  void set_power(bool state);
  void set_zone_power(uint8_t zone, bool state);
  void set_zone_mute(uint8_t zone, bool state);
  void set_zone_volume(uint8_t zone, float value);
  uint8_t get_active_input_channels() { return this->active_input_channels_; }
  uint8_t get_active_output_channels() { return this->active_output_channels_; }
  std::string get_input_config() {
    return channel_config_to_string(
        static_cast<ChannelConfig>(this->active_input_channels_));
  }
  std::string get_output_config() {
    return channel_config_to_string(
        static_cast<ChannelConfig>(this->active_output_channels_));
  }
#ifdef USE_SWITCH
  void set_zone_power_switch(uint8_t zone, switch_::Switch *s);
  void set_zone_mute_switch(uint8_t zone, switch_::Switch *s);
#endif
#ifdef USE_NUMBER
  void set_zone_volume_number(uint8_t zone, number::Number *n);
#endif
#ifdef USE_TEXT_SENSOR
  void set_zone_source_text_sensor(uint8_t zone, text_sensor::TextSensor *s);
#endif

protected:
  void send_command_(const char *cmd);
  void handle_data_(const std::string &data);
  void handle_event_(const std::string &event, ZoneStructure zone,
                     const std::string &parameter);
  void readline_(int readch, uint8_t *buffer, uint8_t len);

  uint8_t buffer_pos_ = 0; // where to resume processing/populating buffer
  uint8_t buffer_data_[MAX_LINE_LENGTH];

  uint8_t active_input_channels_ = 0;
  uint8_t active_output_channels_ = 0;

  uint32_t last_channels_update_ = 0;

#ifdef USE_NUMBER
  std::vector<number::Number *> zone_volume_numbers_ =
      std::vector<number::Number *>(MAX_ZONES);
#endif
#ifdef USE_SWITCH
  std::vector<switch_::Switch *> zone_power_switches_ =
      std::vector<switch_::Switch *>(MAX_ZONES);
  std::vector<switch_::Switch *> zone_mute_switches_ =
      std::vector<switch_::Switch *>(MAX_ZONES);
#endif
#ifdef USE_TEXT_SENSOR
  std::vector<text_sensor::TextSensor *> zone_source_text_sensors_ =
      std::vector<text_sensor::TextSensor *>(MAX_ZONES);
#endif
};

} // namespace denonavr
} // namespace esphome
