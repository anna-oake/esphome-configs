import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client
from esphome.const import CONF_ID, CONF_ON_KEY, CONF_TRIGGER_ID
from esphome import automation


DEPENDENCIES = ["ble_client"]
CODE_OWNERS = ["@fsievers22"]

MULTI_CONF = 3

CONF_ON_STICK = "on_stick"

ble_client_hid_ns = cg.esphome_ns.namespace("ble_client_hid")

BLEClientHID = ble_client_hid_ns.class_(
    "BLEClientHID",
    cg.Component,
    ble_client.BLEClientNode,
)
GamepadStickTrigger = ble_client_hid_ns.class_(
    "GamepadStickTrigger", automation.Trigger.template(cg.float_, cg.float_)
)
GamepadKeyTrigger = ble_client_hid_ns.class_(
    "GamepadKeyTrigger", automation.Trigger.template(cg.std_string, cg.bool_)
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BLEClientHID),
            cv.Optional(CONF_ON_STICK): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(GamepadStickTrigger),
                }
            ),
            cv.Optional(CONF_ON_KEY): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(GamepadKeyTrigger),
                }
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(ble_client.BLE_CLIENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)
    for conf in config.get(CONF_ON_STICK, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
        cg.add(var.register_stick_trigger(trigger))
        await automation.build_automation(
            trigger, [(cg.float_, "angle"), (cg.float_, "velocity")], conf
        )
    for conf in config.get(CONF_ON_KEY, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
        cg.add(var.register_key_trigger(trigger))
        await automation.build_automation(
            trigger, [(cg.std_string, "key"), (cg.bool_, "pressed")], conf
        )
