#include "zone_mute_switch.h"

namespace esphome {
namespace denonavr {

ZoneMuteSwitch::ZoneMuteSwitch(uint8_t zone) : zone_(zone) {}

void ZoneMuteSwitch::write_state(bool state) {
  this->parent_->set_zone_mute(this->zone_, state);
}

} // namespace denonavr
} // namespace esphome
