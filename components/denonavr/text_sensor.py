import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_DIRECTION,
    CONF_MAC_ADDRESS,
    CONF_VERSION,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ENTITY_CATEGORY_NONE,
    ICON_BLUETOOTH,
    ICON_CHIP,
    ICON_SIGN_DIRECTION,
)

from . import CONF_DENONAVR_ID, DenonAVRComponent

CONF_INPUT_CHANNELS = "input_channels"
CONF_OUTPUT_CHANNELS = "output_channels"
CONF_SOURCE = "source"

DEPENDENCIES = ["denonavr"]

MAX_ZONES = 3

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_DENONAVR_ID): cv.use_id(DenonAVRComponent),
        cv.Optional(CONF_INPUT_CHANNELS): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_NONE,
        ),
        cv.Optional(CONF_OUTPUT_CHANNELS): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_NONE,
        ),
    }
)

CONFIG_SCHEMA = CONFIG_SCHEMA.extend(
    {
        cv.Optional(f"zone_{n + 1}"): cv.Schema(
            {
                cv.Optional(CONF_SOURCE): text_sensor.text_sensor_schema(
                    entity_category=ENTITY_CATEGORY_NONE,
                ),
            }
        )
        for n in range(MAX_ZONES)
    }
)


async def to_code(config):
    denonavr_component = await cg.get_variable(config[CONF_DENONAVR_ID])
    if input_channels := config.get(CONF_INPUT_CHANNELS):
        sens = await text_sensor.new_text_sensor(input_channels)
        cg.add(denonavr_component.set_input_channels_text_sensor(sens))
    if output_channels := config.get(CONF_OUTPUT_CHANNELS):
        sens = await text_sensor.new_text_sensor(output_channels)
        cg.add(denonavr_component.set_output_channels_text_sensor(sens))
    for n in range(MAX_ZONES):
        if zone_conf := config.get(f"zone_{n + 1}"):
            if source_config := zone_conf.get(CONF_SOURCE):
                sens = await text_sensor.new_text_sensor(source_config)
                cg.add(denonavr_component.set_zone_source_text_sensor(n, sens))
