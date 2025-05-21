#pragma once

#include "../denonavr.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace denonavr {

class ZoneMuteSwitch : public switch_::Switch,
                       public Parented<DenonAVRComponent> {
public:
  ZoneMuteSwitch(uint8_t zone);

protected:
  uint8_t zone_;
  void write_state(bool state) override;
};

} // namespace denonavr
} // namespace esphome
