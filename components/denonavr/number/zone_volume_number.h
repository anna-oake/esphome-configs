#pragma once

#include "../denonavr.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace denonavr {

class ZoneVolumeNumber : public number::Number,
                         public Parented<DenonAVRComponent> {
public:
  ZoneVolumeNumber(uint8_t zone);

protected:
  uint8_t zone_;
  void control(float value) override;
};

} // namespace denonavr
} // namespace esphome
