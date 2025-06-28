#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "hid_parser.h"
#include <map>

namespace esphome {
namespace ble_client_hid {

namespace espbt = esphome::esp32_ble_tracker;

struct StickCmd {
  float angle;
  float velocity;
};
StickCmd convertXYtoTank(uint32_t Xraw, uint32_t Yraw);

enum class HIDState {
  // Initial state
  INIT,
  SETUP,

  HID_SERVICE_FOUND,

  NO_HID_SERVICE,
  // Client is coonnected
  BLE_CONNECTED,
  // Start reading relevant client characteristics
  READING_CHARS,
  // Finished reaading client characteristics
  READ_CHARS,
  // Configure ble client with read chars e. g. register fr notify
  CONFIGURING,
  // Finished configuring e. g. notify registered
  CONFIGURED,
  // HID opened
  OPENED,
  // HID closed
  CLOSED,
};

enum class GamepadButton {
  NONE = 0,
  A = 2,
  B = 5,
  X = 1,
  Y = 4,
  STICK_X = 48,
  STICK_Y = 49,
  START = 12,
};

class GATTReadData {
public:
  GATTReadData(uint16_t handle, uint8_t *value, uint16_t value_len) {
    this->handle_ = handle;
    this->value_len_ = value_len;
    this->value_ = new uint8_t[value_len];
    memcpy(this->value_, value, sizeof(uint8_t) * value_len);
  }
  ~GATTReadData() { delete value_; }

public:
  uint8_t *value_;
  uint16_t value_len_;
  uint16_t handle_;
};

class GamepadStickTrigger : public Trigger<float, float> {};
class GamepadKeyTrigger : public Trigger<std::string, bool> {};

class BLEClientHID : public Component, public ble_client::BLEClientNode {
public:
  void loop() override;
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;

  void dump_config() override;
  void schedule_read_char(ble_client::BLECharacteristic *characteristic);
  void on_gatt_read_finished(GATTReadData *data);
  void read_client_characteristics();
  float get_setup_priority() const override {
    return setup_priority::AFTER_BLUETOOTH;
  }
  void configure_hid_client();
  void register_stick_trigger(GamepadStickTrigger *trig);
  void register_key_trigger(GamepadKeyTrigger *trig);

protected:
  void send_input_report_event(esp_ble_gattc_cb_param_t *p_data);

  HIDReportMap *hid_report_map;
  std::vector<ble_client::BLECharacteristic *> characteristics;
  std::vector<uint16_t> handles_registered_for_notify;
  std::map<uint16_t, GATTReadData *> handles_to_read;
  std::map<uint16_t, uint8_t> handle_report_id;
  HIDState hid_state = HIDState::INIT;
  int32_t stick_x = 128;
  int32_t stick_y = 128;
  std::vector<GamepadStickTrigger *> stick_triggers_;
  std::vector<GamepadKeyTrigger *> key_triggers_;
};

} // namespace ble_client_hid
} // namespace esphome