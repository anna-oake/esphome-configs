#include "zone_volume_number.h"

namespace esphome {
namespace denonavr {

ZoneVolumeNumber::ZoneVolumeNumber(uint8_t zone) : zone_(zone) {}

void ZoneVolumeNumber::control(float value) {
  this->parent_->set_zone_volume(this->zone_, value);
}

} // namespace denonavr
} // namespace esphome
