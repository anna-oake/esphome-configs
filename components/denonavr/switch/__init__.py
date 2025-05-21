import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_SWITCH,
    ENTITY_CATEGORY_CONFIG,
    ICON_BLUETOOTH,
    ICON_PULSE,
)

from .. import CONF_DENONAVR_ID, DenonAVRComponent, denonavr_ns

CONF_MUTE = "mute"
CONF_POWER = "power"
MAX_ZONES = 3

PowerSwitch = denonavr_ns.class_("PowerSwitch", switch.Switch)
ZoneMuteSwitch = denonavr_ns.class_("ZoneMuteSwitch", switch.Switch)
ZonePowerSwitch = denonavr_ns.class_("ZonePowerSwitch", switch.Switch)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_DENONAVR_ID): cv.use_id(DenonAVRComponent),
        cv.Optional(CONF_POWER): switch.switch_schema(
            PowerSwitch,
            device_class=DEVICE_CLASS_SWITCH,
        ),
    }
)

CONFIG_SCHEMA = CONFIG_SCHEMA.extend(
    {
        cv.Optional(f"zone_{n + 1}"): cv.Schema(
            {
                cv.Optional(CONF_MUTE): switch.switch_schema(
                    ZoneMuteSwitch,
                    device_class=DEVICE_CLASS_SWITCH,
                ),
                cv.Optional(CONF_POWER): switch.switch_schema(
                    ZonePowerSwitch,
                    device_class=DEVICE_CLASS_SWITCH,
                ),
            }
        )
        for n in range(MAX_ZONES)
    }
)


async def to_code(config):
    denonavr_component = await cg.get_variable(config[CONF_DENONAVR_ID])
    if power_config := config.get(CONF_POWER):
        s = await switch.new_switch(power_config)
        await cg.register_parented(s, config[CONF_DENONAVR_ID])
        cg.add(denonavr_component.set_power_switch(s))
    for zone_num in range(MAX_ZONES):
        zone_num += 1
        if zone_conf := config.get(f"zone_{zone_num}"):
            if zone_mute_config := zone_conf.get(CONF_MUTE):
                mute = cg.new_Pvariable(zone_mute_config[CONF_ID], zone_num)
                await switch.register_switch(mute, zone_mute_config)
                await cg.register_parented(mute, config[CONF_DENONAVR_ID])
                cg.add(denonavr_component.set_zone_mute_switch(zone_num, mute))
            if zone_power_config := zone_conf.get(CONF_POWER):
                power = cg.new_Pvariable(zone_power_config[CONF_ID], zone_num)
                await switch.register_switch(power, zone_power_config)
                await cg.register_parented(power, config[CONF_DENONAVR_ID])
                cg.add(denonavr_component.set_zone_power_switch(zone_num, power))
