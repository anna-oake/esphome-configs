import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@anna-oake"]
MULTI_CONF = True

denonavr_ns = cg.esphome_ns.namespace("denonavr")
DenonAVRComponent = denonavr_ns.class_("DenonAVRComponent", cg.Component, uart.UARTDevice)

CONF_DENONAVR_ID = "denonavr_id"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DenonAVRComponent),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)

DenonAVRBaseSchema = cv.Schema(
    {
        cv.GenerateID(CONF_DENONAVR_ID): cv.use_id(DenonAVRComponent),
    },
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "denonavr",
    require_tx=True,
    require_rx=True,
    parity="NONE",
    stop_bits=1,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
