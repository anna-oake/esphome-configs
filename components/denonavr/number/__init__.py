import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import CONF_ID

from .. import CONF_DENONAVR_ID, DenonAVRComponent, denonavr_ns

CONF_VOLUME = "volume"
MAX_ZONES = 3

ZoneVolumeNumber = denonavr_ns.class_("ZoneVolumeNumber", number.Number)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_DENONAVR_ID): cv.use_id(DenonAVRComponent),
    }
)

CONFIG_SCHEMA = CONFIG_SCHEMA.extend(
    {
        cv.Optional(f"zone_{n + 1}"): cv.Schema(
            {
                cv.Optional(CONF_VOLUME): number.number_schema(
                    ZoneVolumeNumber,
                ),
            }
        )
        for n in range(MAX_ZONES)
    }
)


async def to_code(config):
    denonavr_component = await cg.get_variable(config[CONF_DENONAVR_ID])
    for zone_num in range(MAX_ZONES):
        zone_num += 1
        if zone_conf := config.get(f"zone_{zone_num}"):
            if zone_volume_config := zone_conf.get(CONF_VOLUME):
                volume = cg.new_Pvariable(zone_volume_config[CONF_ID], zone_num)
                await number.register_number(
                    volume, zone_volume_config, min_value=0, max_value=98, step=0.5
                )
                await cg.register_parented(volume, config[CONF_DENONAVR_ID])
                cg.add(denonavr_component.set_zone_volume_number(zone_num, volume))
