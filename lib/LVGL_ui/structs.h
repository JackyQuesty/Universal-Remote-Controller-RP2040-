#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_INPUT_MONITOR_STRUCT = 16384
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_INPUT_MONITOR_STRUCT = 81920
};

enum Input_Monitor_structFlowStructureFields {
    FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_YAW = 0,
    FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_THROTTLE = 1,
    FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_PITCH = 2,
    FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_ROLL = 3,
    FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_NUM_FIELDS
};

struct Input_Monitor_structValue {
    Value value;
    
    Input_Monitor_structValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_NUM_FIELDS, FLOW_STRUCTURE_INPUT_MONITOR_STRUCT, 0);
    }
    
    Input_Monitor_structValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    int Yaw() {
        return value.getArray()->values[FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_YAW].getInt();
    }
    void Yaw(int Yaw) {
        value.getArray()->values[FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_YAW] = IntegerValue(Yaw);
    }
    
    int Throttle() {
        return value.getArray()->values[FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_THROTTLE].getInt();
    }
    void Throttle(int Throttle) {
        value.getArray()->values[FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_THROTTLE] = IntegerValue(Throttle);
    }
    
    int Pitch() {
        return value.getArray()->values[FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_PITCH].getInt();
    }
    void Pitch(int Pitch) {
        value.getArray()->values[FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_PITCH] = IntegerValue(Pitch);
    }
    
    int Roll() {
        return value.getArray()->values[FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_ROLL].getInt();
    }
    void Roll(int Roll) {
        value.getArray()->values[FLOW_STRUCTURE_INPUT_MONITOR_STRUCT_FIELD_ROLL] = IntegerValue(Roll);
    }
};

typedef ArrayOf<Input_Monitor_structValue, FLOW_ARRAY_OF_STRUCTURE_INPUT_MONITOR_STRUCT> ArrayOfInput_Monitor_structValue;

#endif /*EEZ_LVGL_UI_STRUCTS_H*/