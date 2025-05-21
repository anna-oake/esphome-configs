#include "power_switch.h"

namespace esphome {
namespace denonavr {

void PowerSwitch::write_state(bool state) { this->parent_->set_power(state); }

} // namespace denonavr
} // namespace esphome
