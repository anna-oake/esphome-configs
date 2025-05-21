#pragma once

#include "../denonavr.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace denonavr {

class PowerSwitch : public switch_::Switch, public Parented<DenonAVRComponent> {
public:
  PowerSwitch() = default;

protected:
  void write_state(bool state) override;
};

} // namespace denonavr
} // namespace esphome
