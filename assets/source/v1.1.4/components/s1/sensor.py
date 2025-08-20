import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_UART_ID
from esphome.components import sensor, uart, number, switch as _switch, text_sensor

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "text_sensor"]

ns = cg.esphome_ns.namespace("s1")
LD2450 = ns.class_("LD2450", cg.Component, uart.UARTDevice)

CONF_DETECTION_RANGE = "detection_range"
CONF_FLIP_Y = "flip_y"
CONF_EXC_X_BEGIN = "exclusion_x_begin"
CONF_EXC_X_END = "exclusion_x_end"
CONF_EXC_Y_BEGIN = "exclusion_y_begin"
CONF_EXC_Y_END = "exclusion_y_end"
CONF_TRACKING_MODE = "tracking_mode"

SENSOR_KEYS = [
    "target1_x", "target1_y", "target1_angle", "target1_speed", "target1_distance",
    "target2_x", "target2_y", "target2_angle", "target2_speed", "target2_distance",
    "target3_x", "target3_y", "target3_angle", "target3_speed", "target3_distance",
]

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LD2450),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),

    **{cv.Required(key): sensor.sensor_schema() for key in SENSOR_KEYS},

    cv.Required(CONF_DETECTION_RANGE): cv.use_id(number.Number),
    cv.Required(CONF_FLIP_Y): cv.use_id(_switch.Switch),

    cv.Required(CONF_EXC_X_BEGIN): cv.use_id(number.Number),
    cv.Required(CONF_EXC_X_END): cv.use_id(number.Number),
    cv.Required(CONF_EXC_Y_BEGIN): cv.use_id(number.Number),
    cv.Required(CONF_EXC_Y_END): cv.use_id(number.Number),

    cv.Required(CONF_TRACKING_MODE): text_sensor.text_sensor_schema(),

}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    for key in SENSOR_KEYS:
        await sensor.register_sensor(getattr(var, key), config[key])

    dr = await cg.get_variable(config[CONF_DETECTION_RANGE])
    cg.add(var.set_detection_range(dr))

    fy = await cg.get_variable(config[CONF_FLIP_Y])
    cg.add(var.set_flip_y(fy))

    xb = await cg.get_variable(config[CONF_EXC_X_BEGIN])
    xe = await cg.get_variable(config[CONF_EXC_X_END])
    yb = await cg.get_variable(config[CONF_EXC_Y_BEGIN])
    ye = await cg.get_variable(config[CONF_EXC_Y_END])
    cg.add(var.set_exclusion_x_begin(xb))
    cg.add(var.set_exclusion_x_end(xe))
    cg.add(var.set_exclusion_y_begin(yb))
    cg.add(var.set_exclusion_y_end(ye))

    tracking = await text_sensor.new_text_sensor(config[CONF_TRACKING_MODE])
    cg.add(var.set_tracking_mode_sensor(tracking))