#include "zone_power_switch.h"

namespace esphome {
namespace denonavr {

ZonePowerSwitch::ZonePowerSwitch(uint8_t zone) : zone_(zone) {}

void ZonePowerSwitch::write_state(bool state) {
  this->parent_->set_zone_power(this->zone_, state);
}

} // namespace denonavr
} // namespace esphome
