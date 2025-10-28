import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_UART_ID
from esphome.components import sensor, uart, number, switch as _switch, text_sensor

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "text_sensor", "number"]

ns = cg.esphome_ns.namespace("s1")
LD2450 = ns.class_("LD2450", cg.Component, uart.UARTDevice)
CONF_DETECTION_RANGE = "detection_range"
CONF_FLIP_Y = "flip_y"
CONF_TRACKING_MODE = "tracking_mode"
CONF_BLUETOOTH_STATE = "bluetooth_state"
CONF_TARGET1_STATE = "target1_state"
CONF_TARGET2_STATE = "target2_state"
CONF_TARGET3_STATE = "target3_state"
CONF_EXCLUSION_ZONE_POINTS_COUNT = "exclusion_zone_points_count"
EXCLUSION_ZONE_KEYS = [f"exclusion_zone_p{i}_{axis}" for i in range(1, 9) for axis in "xy"]
CONF_GATE_RADIUS_CM = "gate_radius_cm"
CONF_STATIONARY_SPEED_THRESH = "stationary_speed_thresh"
CONF_STATIONARY_TIME_S = "stationary_time_s"
CONF_DROPOUT_HOLD_M = "dropout_hold_m"
CONF_DROPOUT_HOLD_S = "dropout_hold_s"
CONF_HOLDING_ENABLED = "holding_enabled"

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
    cv.Required(CONF_TRACKING_MODE): text_sensor.text_sensor_schema(),
    cv.Optional(CONF_BLUETOOTH_STATE): text_sensor.text_sensor_schema(),
    cv.Optional(CONF_TARGET1_STATE): text_sensor.text_sensor_schema(),
    cv.Optional(CONF_TARGET2_STATE): text_sensor.text_sensor_schema(),
    cv.Optional(CONF_TARGET3_STATE): text_sensor.text_sensor_schema(),
    cv.Optional(CONF_EXCLUSION_ZONE_POINTS_COUNT): cv.use_id(number.Number),
    **{cv.Optional(key): cv.use_id(number.Number) for key in EXCLUSION_ZONE_KEYS},
    cv.Optional(CONF_GATE_RADIUS_CM): cv.use_id(number.Number),
    cv.Optional(CONF_STATIONARY_SPEED_THRESH): cv.use_id(number.Number),
    cv.Optional(CONF_STATIONARY_TIME_S): cv.use_id(number.Number),
    cv.Optional(CONF_DROPOUT_HOLD_M): cv.use_id(number.Number),
    cv.Optional(CONF_DROPOUT_HOLD_S): cv.use_id(number.Number),
    cv.Optional(CONF_HOLDING_ENABLED): cv.use_id(_switch.Switch),

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

    tracking = await text_sensor.new_text_sensor(config[CONF_TRACKING_MODE])
    cg.add(var.set_tracking_mode_sensor(tracking))

    if CONF_BLUETOOTH_STATE in config:
        bt_state = await text_sensor.new_text_sensor(config[CONF_BLUETOOTH_STATE])
        cg.add(var.set_bluetooth_state_sensor(bt_state))

    if CONF_TARGET1_STATE in config:
        t1state = await text_sensor.new_text_sensor(config[CONF_TARGET1_STATE])
        cg.add(var.set_target1_state_sensor(t1state))

    if CONF_TARGET2_STATE in config:
        t2state = await text_sensor.new_text_sensor(config[CONF_TARGET2_STATE])
        cg.add(var.set_target2_state_sensor(t2state))

    if CONF_TARGET3_STATE in config:
        t3state = await text_sensor.new_text_sensor(config[CONF_TARGET3_STATE])
        cg.add(var.set_target3_state_sensor(t3state))

    if CONF_EXCLUSION_ZONE_POINTS_COUNT in config:
        count_var = await cg.get_variable(config[CONF_EXCLUSION_ZONE_POINTS_COUNT])
        cg.add(var.set_exclusion_zone_points_count(count_var))

    for key in EXCLUSION_ZONE_KEYS:
        if key in config:
            point_var = await cg.get_variable(config[key])
            setter_name = "set_" + key
            cg.add(getattr(var, setter_name)(point_var))

    if CONF_GATE_RADIUS_CM in config:
        gate = await cg.get_variable(config[CONF_GATE_RADIUS_CM])
        cg.add(var.set_gate_radius_cm(gate))

    if CONF_STATIONARY_SPEED_THRESH in config:
        vthr = await cg.get_variable(config[CONF_STATIONARY_SPEED_THRESH])
        cg.add(var.set_stationary_speed_thresh(vthr))

    if CONF_STATIONARY_TIME_S in config:
        stat_t = await cg.get_variable(config[CONF_STATIONARY_TIME_S])
        cg.add(var.set_stationary_time_s(stat_t))

    if CONF_DROPOUT_HOLD_M in config:
        hold_m = await cg.get_variable(config[CONF_DROPOUT_HOLD_M])
        cg.add(var.set_dropout_hold_m(hold_m))


    elif CONF_DROPOUT_HOLD_S in config:
        hold_s_legacy = await cg.get_variable(config[CONF_DROPOUT_HOLD_S])
        cg.add(var.set_dropout_hold_m(hold_s_legacy))

    if CONF_HOLDING_ENABLED in config:
        hold_enabled = await cg.get_variable(config[CONF_HOLDING_ENABLED])
        cg.add(var.set_holding_enabled(hold_enabled))