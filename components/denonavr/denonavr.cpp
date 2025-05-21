#include "denonavr.h"
#include "utils.h"
#include <bitset>
#include <utility>
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#include "esphome/core/component.h"

namespace esphome {
namespace denonavr {

static const char *const TAG = "denonavr";

DenonAVRComponent::DenonAVRComponent() {}

void DenonAVRComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Denon AVR...");
  const std::vector<std::string> probe_cmds = {
      "PW?",        "ZM?",     "SI?",       "MV?",         "MU?",
      "Z2?",        "Z2MU?",   "Z3?",       "Z3MU?",       "PSTONE CTRL ?",
      "PSBAS ?",    "PSTRE ?", "PSDYNEQ ?", "PSMULTEQ: ?", "PSREFLEV ?",
      "PSDYNVOL ?", "MS?",     "MNMEN?",    "OPINFINS ?",  "OPINFASP ?"};
  for (const auto &cmd : probe_cmds) {
    this->send_command_(cmd.c_str());
  }
}

void DenonAVRComponent::dump_config() { ESP_LOGCONFIG(TAG, "Denon AVR:"); }

void DenonAVRComponent::loop() {
  while (this->available()) {
    this->readline_(read(), this->buffer_data_, MAX_LINE_LENGTH);
  }

  if (millis() - this->last_channels_update_ > 2000) {
    this->last_channels_update_ = millis();
    this->send_command_("OPINFINS ?");
    this->send_command_("OPINFASP ?");
  }
}

void DenonAVRComponent::set_power(bool state) {
  ESP_LOGD(TAG, "Set power %d", state);
  this->send_command_(state ? "PWON" : "PWSTANDBY");
}

void DenonAVRComponent::set_zone_power(uint8_t zone, bool state) {
  ESP_LOGD(TAG, "Set power %d for zone %d", state, zone);
  std::string cmd = "";
  switch (zone) {
  case 1:
    cmd = "ZM";
    break;
  case 2:
    cmd = "Z2";
    break;
  case 3:
    cmd = "Z3";
    break;
  default:
    return;
  }
  cmd += state ? "ON" : "OFF";
  this->send_command_(cmd.c_str());
}

void DenonAVRComponent::set_zone_mute(uint8_t zone, bool state) {
  ESP_LOGD(TAG, "Set mute %d for zone %d", state, zone);
  std::string cmd = "";
  switch (zone) {
  case 1:
    cmd = "MU";
    break;
  case 2:
    cmd = "Z2MU";
    break;
  case 3:
    cmd = "Z3MU";
    break;
  default:
    return;
  }
  cmd += state ? "ON" : "OFF";
  this->send_command_(cmd.c_str());
}

void DenonAVRComponent::set_zone_volume(uint8_t zone, float value) {
  ESP_LOGD(TAG, "Set volume %0.1f for zone %d", value, zone);
  std::string cmd = "";
  switch (zone) {
  case 1:
    cmd = "MV";
    break;
  case 2:
    cmd = "Z2";
    break;
  case 3:
    cmd = "Z3";
    break;
  default:
    return;
  }
  cmd += format_volume(value);
  this->send_command_(cmd.c_str());
}

void DenonAVRComponent::send_command_(const char *cmd) {
  ESP_LOGV(TAG, "Sending command %s", cmd);
  this->write_str(cmd);
  this->write_byte(0x0D); // "/r"
  // FIXME to remove
  delay(50); // NOLINT
}

void DenonAVRComponent::handle_event_(const std::string &event,
                                      ZoneStructure zone,
                                      const std::string &parameter) {
  if (event == "MV") {
    int value = std::stoi(parameter);
    float volume = parameter.size() == 3 ? value / 10.0f : value;
    ESP_LOGD(TAG, "Zone %d, Volume: %0.1f", zone, volume);
#ifdef USE_NUMBER
    if (this->zone_volume_numbers_[zone - 1] != nullptr) {
      this->zone_volume_numbers_[zone - 1]->publish_state(volume);
    }
#endif
    return;
  }

  if (event == "SI") {
    ESP_LOGD(TAG, "Zone %d, Source: %s", zone, parameter.c_str());
#ifdef USE_TEXT_SENSOR
    if (this->zone_source_text_sensors_[zone - 1] != nullptr) {
      if (this->zone_source_text_sensors_[zone - 1]->state != parameter) {
        this->zone_source_text_sensors_[zone - 1]->publish_state(parameter);
      }
    }
#endif
    return;
  }

  if (event == "PW") {
    ESP_LOGD(TAG, "Power: %s", parameter.c_str());
#ifdef USE_SWITCH
    if (this->power_switch_ != nullptr) {
      this->power_switch_->publish_state(parameter == "ON");
    }
#endif
    return;
  }

  if (event == "MU") {
    ESP_LOGD(TAG, "Zone %d, Mute: %s", zone, parameter.c_str());
#ifdef USE_SWITCH
    if (this->zone_mute_switches_[zone - 1] != nullptr) {
      this->zone_mute_switches_[zone - 1]->publish_state(parameter == "ON");
    }
#endif
    return;
  }

  if ((event == "ZM" || event == "Z2" || event == "Z3") &&
      (parameter == "ON" || parameter == "OFF")) {
#ifdef USE_SWITCH
    if (this->zone_power_switches_[zone - 1] != nullptr) {
      this->zone_power_switches_[zone - 1]->publish_state(parameter == "ON");
    }
#endif
    return;
  }

  if (event == "OP") {
    if (parameter.size() < 8) {
      return;
    }
    auto param = parameter.substr(0, 6);
    if (param != "INFINS" && param != "INFASP") {
      return;
    }
    auto channels = parameter.substr(7, 8);
    std::bitset<8> bs;
    for (size_t i = 0; i < 8; ++i) {
      bs[i] = (channels[i] == '2');
    }
    if (param == "INFINS") {
      this->active_input_channels_ = static_cast<uint8_t>(bs.to_ulong());
#ifdef USE_TEXT_SENSOR
      if (this->input_channels_text_sensor_ != nullptr) {
        auto cfg = this->get_input_config();
        if (cfg != this->input_channels_text_sensor_->state) {
          this->input_channels_text_sensor_->publish_state(cfg);
        }
      }
#endif
    } else if (param == "INFASP") {
      this->active_output_channels_ = static_cast<uint8_t>(bs.to_ulong());
#ifdef USE_TEXT_SENSOR
      if (this->output_channels_text_sensor_ != nullptr) {
        auto cfg = this->get_output_config();
        if (cfg != this->output_channels_text_sensor_->state) {
          this->output_channels_text_sensor_->publish_state(cfg);
        }
      }
#endif
    }
    return;
  }

  ESP_LOGD(TAG, "Event: %s, Zone: %d, Parameter: %s", event.c_str(), zone,
           parameter.c_str());
}

void DenonAVRComponent::handle_data_(const std::string &data) {
  if (data.size() < 3) {
    return;
  }
  ESP_LOGV(TAG, "Received data: %s", data.c_str());

  std::string event = "";

  auto it =
      std::find_if(EVENTS.begin(), EVENTS.end(), [&](std::string_view prefix) {
        return data.rfind(prefix, 0) == 0;
      });
  if (it != EVENTS.end()) {
    event = *it;
  }

  auto parameter = data.substr(event.size());

  auto zone = ZONE_MAIN;
  if (in_array(event, ALL_ZONE_EVENTS)) {
    zone = ZONE_ALL;
  } else if (event == "Z2" || event == "Z3") {
    if (event == "Z2") {
      zone = ZONE_2;
    } else {
      zone = ZONE_3;
    }
    if (in_array(parameter, SOURCES)) {
      event = "SI";
    } else if (is_integer(parameter)) {
      event = "MV";
    } else {
      auto it = std::find_if(EVENTS.begin(), EVENTS.end(),
                             [&](std::string_view prefix) {
                               return parameter.rfind(prefix, 0) == 0;
                             });
      if (it != EVENTS.end()) {
        event = *it;
        parameter = parameter.substr(event.size());
      }
    }
  }

  trim(event);
  trim(parameter);

  if (!in_array(event, EVENTS)) {
    ESP_LOGW(TAG, "Unknown event: %s", data.c_str());
    return;
  }

  this->handle_event_(event, zone, parameter);
}

void DenonAVRComponent::readline_(int readch, uint8_t *buffer, uint8_t len) {
  if (readch < 0) {
    return;
  }
  if (this->buffer_pos_ < len - 1) {
    buffer[this->buffer_pos_++] = readch;
    buffer[this->buffer_pos_] = 0;
  } else {
    this->buffer_pos_ = 0;
  }
  if (this->buffer_pos_ < 3) {
    return;
  }
  if (buffer[this->buffer_pos_ - 1] == 0x0D) {
    ESP_LOGV(TAG, "Handle incoming data");
    std::string data(reinterpret_cast<const char *>(buffer),
                     this->buffer_pos_ - 1);
    this->handle_data_(data);
    this->buffer_pos_ = 0;
  }
}

#ifdef USE_SWITCH
void DenonAVRComponent::set_zone_power_switch(uint8_t zone,
                                              switch_::Switch *s) {
  this->zone_power_switches_[zone - 1] = s;
}
void DenonAVRComponent::set_zone_mute_switch(uint8_t zone, switch_::Switch *s) {
  this->zone_mute_switches_[zone - 1] = s;
}
#endif

#ifdef USE_NUMBER
void DenonAVRComponent::set_zone_volume_number(uint8_t zone,
                                               number::Number *n) {
  this->zone_volume_numbers_[zone - 1] = n;
}
#endif

#ifdef USE_TEXT_SENSOR
void DenonAVRComponent::set_zone_source_text_sensor(
    uint8_t zone, text_sensor::TextSensor *s) {
  this->zone_source_text_sensors_[zone] = s;
}
#endif

} // namespace denonavr
} // namespace esphome
