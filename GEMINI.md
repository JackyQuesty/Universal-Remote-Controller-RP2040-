# Universal Controller - Main MCU (RP2040) Project Documentation & Rules

This document serves as the persistent context, architecture specification, and developer rules for the **Custom Universal Remote Control - Main MCU Firmware**.

---

## 1. Agent Behavior Rules & Development Protocols

### Rule 1: Session Start & Auto Context Ingestion
- At the start of every session, acknowledge the project context, target hardware specifications, active pinouts, and the current state of pending/verified modules.
- Check for any user edits on disk before proposing modifications.

### Rule 2: Strict Workspace Boundary (Strict Isolation)
- Keep all operations contained within the workspace root (`Universal_Controller_MainMCU_Code_pio/`).
- Do not inspect, create, or alter files outside this directory unless specifically instructed by the user.

### Rule 3: Hardware Verification Lifecycle & Status Protocol
Embedded code behaves differently in hardware than in simulation. Always adhere to the verification state lifecycle:
- **`[COMPILED - PENDING HARDWARE TEST]`**: Firmware compiles cleanly with no compiler warnings/errors, but physical bench verification has not been confirmed.
- **`[VERIFIED ON HARDWARE]`**: The user has explicitly confirmed physical functionality on the bench.
- **`[HARDWARE BUG / BLOCKED]`**: Physical testing revealed a hardware or runtime malfunction. The symptom, cause, and remediation must be logged in the *Known Hardware Quirks & Debug Notes* section.
- **Never claim a feature works on real silicon without user confirmation.**

### Rule 4: User Manual Edits & Code Sync Protocol
- Always read the on-disk state of files before editing to prevent accidental rollbacks of user changes.
- If the user modifies pin numbers, ISR handlers, timing, or task structures, review changes for RP2040 safety (e.g. core affinities, mutex contention, ISR execution limits).

### Rule 5: Continuous Maintenance Protocol
- Proactively update `GEMINI.md` whenever pins, libraries, protocol definitions, or task structures change.

---

## 2. Project Overview & System Architecture

The **Custom Universal Remote Controller** is a modular multi-purpose handheld controller designed for:
1. **Drone Flight Control (ESP-NOW / FC Profile)**: Quadcopter remote control streaming Roll, Pitch, Throttle, Yaw, and AUX1–AUX4 channels.
2. **PC / Host Gamepad & Input Device (BLE Controller)**: BLE HID Mouse and Keyboard simulation.
3. **Robotics Control (4WD Mecanum)**: Holonomic mobile robot platform control.
4. **General RF Link (NRF24)**: Long-range RF transceiver link.

### Dual-MCU Architecture
- **Main MCU (RP2040 / Raspberry Pi Pico - This Project)**:
  - High-speed analog sensor acquisition (dual 12-bit I2C ADCs).
  - Button expander interrupt management (10 digital buttons).
  - Dual-core FreeRTOS task scheduling.
  - Graphical UI execution via LovyanGFX, LVGL 9.5.0, and EEZ-Studio.
  - Custom UART transmission to the Secondary MCU.
- **Secondary MCU (ESP32-C3 - External Board)**:
  - Dedicated wireless co-processor handling BLE HID, ESP-NOW, and NRF24 RF links.

```
+-----------------------------------------------------------------------------------------+
|                                    RP2040 MAIN MCU                                      |
|                                                                                         |
|  +-------------------------------------+       +-------------------------------------+  |
|  |             CORE 0                  |       |             CORE 1                  |  |
|  |     (Inputs & Communications)       |       |       (Display & User Interface)    |  |
|  |                                     |       |                                     |  |
|  |  +-------------------------------+  |       |  +-------------------------------+  |  |
|  |  | task_InputRead (100 Hz)       |  |       |  | core1_loop (200 Hz)           |  |  |
|  |  | - ADS1015 L/R Joysticks (I2C) |  |       |  | - LVGL v9.5.0 timer handler   |  |  |
|  |  | - PCF8575 Button Matrix (IRQ) |  |       |  | - LovyanGFX ST7789 flush      |  |  |
|  |  +---------------+---------------+  |       |  | - XPT2046 touch polling       |  |  |
|  |                  |                  |       |  | - EEZ-Flow variable sync      |  |  |
|  |  +---------------v---------------+  |       |  +---------------+---------------+  |  |
|  |  | task_UART_Handle (100 Hz)     |  |       |                  ^                  |  |
|  |  | - Format packet buffer        |  |       |  +---------------+---------------+  |  |
|  |  | - Stream over Serial2 (UART)  |  |       |  | CoreData_Receive (50 Hz)      |  |  |
|  |  +---------------+---------------+  |       |  | - Reads Core_Data_Drone_FC    |  |  |
|  |                  |                  |       |  +---------------+---------------+  |  |
|  |  +---------------v---------------+  |       |                  ^                  |  |
|  |  | CoreData_Send (50 Hz)         |  |       |                  |                  |  |
|  |  | - Writes Core_Data_Drone_FC   +==+=======+==================+                  |  |
|  |  +-------------------------------+  |  FreeRTOS Mutex                             |  |
|  +-------------------------------------+  (Core_Data_Mutex)                          |  |
+------------------------------------+----------------------------------------------------+
                                     | UART2 (TX: GPIO 20, RX: GPIO 21) @ 115200 baud
                                     v
                  +--------------------------------------+
                  |          ESP32-C3 SECONDARY          |
                  |          WIRELESS TRANSCEIVER        |
                  |     (BLE HID, ESP-NOW, NRF24)        |
                  +--------------------------------------+
```

---

## 3. Hardware & Pinout Specifications

### RP2040 Pinout Configuration

| Peripheral / Interface | Signal Name | RP2040 GPIO | Operating Mode / Notes |
| :--- | :--- | :--- | :--- |
| **ST7789 Display (SPI0)** | `TFT_CS` | **GPIO 0** | Chip Select (Active Low) |
| **ST7789 Display (SPI0)** | `TFT_RST` | **GPIO 1** | Display Hardware Reset |
| **ST7789 Display (SPI0)** | `TFT_DC` | **GPIO 2** | Data / Command Select |
| **Shared SPI0 Bus** | `SPI_MOSI` | **GPIO 3** | Master Out Slave In |
| **Shared SPI0 Bus** | `SPI_MISO` | **GPIO 4** | Master In Slave Out |
| **Shared SPI0 Bus** | `SPI_SCL` | **GPIO 6** | SPI Clock (40MHz TFT write / 2MHz Touch) |
| **XPT2046 Touch (SPI0)** | `TOUCH_CS` | **GPIO 7** | Touch Controller Chip Select |
| **XPT2046 Touch (SPI0)** | `TOUCH_IRQ` | **GPIO 8** | Touch Interrupt Line |
| **I2C Bus (Wire)** | `I2C_SDA` | **GPIO 16** | 400 kHz Fast-Mode Data |
| **I2C Bus (Wire)** | `I2C_SCL` | **GPIO 17** | 400 kHz Fast-Mode Clock |
| **PCF8575 Expander** | `PCF_IRQ` | **GPIO 18** | Interrupt line, `INPUT_PULLUP`, Falling-edge trigger |
| **Secondary MCU UART (Serial2)**| `UART_TX` | **GPIO 20** | RP2040 TX -> ESP32-C3 RX |
| **Secondary MCU UART (Serial2)**| `UART_RX` | **GPIO 21** | RP2040 RX <- ESP32-C3 TX |
| **Flow Control** | `UART_CTS` | **GPIO 22** | Clear To Send (Hardware flow control reserved) |
| **Status LED** | `OnB_led` | **GPIO 25** | Raspberry Pi Pico onboard green LED |

### I2C Bus Device Map

| I2C Address | Device IC | Role / Description |
| :--- | :--- | :--- |
| **`0x48`** (`0b1001000`) | **Adafruit ADS1015 (Left)** | 12-bit ADC for Left Joystick (`AIN0`: Throttle/Y, `AIN1`: Yaw/X) |
| **`0x49`** (`0b1001001`) | **Adafruit ADS1015 (Right)** | 12-bit ADC for Right Joystick (`AIN0`: Pitch/Y, `AIN1`: Roll/X) |
| **`0x20`** | **NXP PCF8575** | 16-bit I/O Expander for Left and Right 5-way D-pads / Gamepads |

### PCF8575 Button Matrix Mapping

Buttons pull to Ground when pressed (Active Low, decoded via bitwise NOT `!(array >> pin & 1)`):

* **Left Gamepad (GamepadL):**
  - Bit 0: Left
  - Bit 1: Center
  - Bit 2: Down
  - Bit 3: Up
  - Bit 4: Right
* **Right Gamepad (GamepadR):**
  - Bit 11: Left
  - Bit 12: Center
  - Bit 13: Down
  - Bit 14: Up
  - Bit 15: Right

---

## 4. Firmware Subsystems & Implementation Details

### Core 0: Backend Tasks (Real-Time Inputs & Comms)
1. **Input Acquisition (`task_InputRead` @ 100 Hz, Stack: 2048 words)**:
   - Samples Left & Right ADCs through `Adafruit_ADS1015` instances configured with `GAIN_ONE` and `RATE_ADS1015_3300SPS`.
   - On PCF8575 falling edge interrupt (`PCF_INT_Flag == true`), reads full 16-bit word from PCF8575 and unpacks button states.
   - Updates `Button` debounce state machines (`UpL`, `DownL`, `UpR`, `DownR`).
   - Toggles onboard LED as heartbeat.
2. **Communications Handler (`task_UART_Handle` @ 100 Hz, Stack: 2048 words)**:
   - Evaluates active protocol mode (`ESP_FC`, `BLE_CONTR`, `IDLE`).
   - Formats payload buffer using custom packet framing.
   - Emits frame via `CUSTOM_UART_t::process_data()` over `Serial2` at 115200 baud.
3. **Core Data Sync (`CoreData_Send` @ 50 Hz, Stack: 2048 words)**:
   - Takes `Core_Data_Mutex` (5ms timeout) and publishes `Local_Data_Core0` into `Core_Data_Drone_FC`.

### Core 1: Frontend Tasks (Graphics & User Interface)
1. **Graphics Engine & GUI Loop (`core1_loop` @ 200 Hz / 5 ms delay, Stack: 4096 words)**:
   - Runs `lv_timer_handler()` and EEZ-GUI `ui_tick()`.
   - When the active screen is `objects.drone_monitor`, pushes live telemetry data (`Throttle`, `Yaw`, `Pitch`, `Roll`, `AUX1-4`) directly into EEZ native variable getters/setters (`vars.h`).
2. **Core Data Ingestion (`CoreData_Receive` @ 50 Hz, Stack: 1024 words)**:
   - Takes `Core_Data_Mutex` (10ms timeout) and pulls `Core_Data_Drone_FC` into `Local_Data_Core1`.

### Joystick Processing & Math Model
- **Calibration (`center_calib`)**: Samples 100 raw ADC values at startup to compute dynamic zero-center offsets (`raw_x_center`, `raw_y_center`).
- **Deadband**: Applies a deadband threshold (`raw_threshold = 10`) around center.
- **Dynamic Auto-Scaling**: Expands `raw_min` and `raw_max` dynamically if joystick deflection exceeds calibrated extremes.
- **Flight Controller Scaling (`JDC` function)**:
  - Normal axes: Maps joystick input (-127 to +127) to RC range 100–200 (center 150), multiplied by 10 in profile struct (1000–2000 µs standard servo pulses).
  - Throttle axis: Maps 0–127 to 100–200 (1000–2000 µs), no auto-centering.

---

## 5. Communications Protocol Specification (UART)

The Main MCU communicates with the Secondary MCU using a framed binary structure:

```
+------------+------------+------------+-------------------------------+
| HEADER(1B) |  MODE(1B)  | LENGTH(1B) |          PAYLOAD(NB)          |
+------------+------------+------------+-------------------------------+
    0xAA         Mode ID      Length         Data bytes (max 20 bytes)
```

### Protocol Modes (`UART_MODE_t`)
- **`IDLE (0x00)`**: Idle / standby state.
- **`BLE_CONTR (0x01)`**: 13-byte payload:
  - `keyboard_data[8]`: HID modifier keys, reserved, keycodes (WASD, Space, Q, E).
  - `mouse_data[5]`: Mouse button mask (Left/Right click), X delta (-7 to +7), Y delta (-7 to +7), wheel bytes.
- **`ESP_FC (0x02)`**: 8-byte payload:
  - Byte 0: Roll (100–200)
  - Byte 1: Pitch (100–200)
  - Byte 2: Throttle (100–200)
  - Byte 3: Yaw (100–200)
  - Byte 4: AUX1 (100 or 200 - Toggle Switch)
  - Byte 5: AUX2 (100 or 200 - Toggle Switch)
  - Byte 6: AUX3 (100 or 200 - Toggle Switch)
  - Byte 7: AUX4 (100 or 200 - Toggle Switch)
- **`NRF24 (0x03)`**: Dedicated RF packet bridge.

---

## 6. Build Environment & Toolchain Details

### PlatformIO Configuration (`platformio.ini`)
```ini
[env:pico]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = pico
framework = arduino
board_build.core = earlephilhower
lib_deps = 
    lovyan03/LovyanGFX@^1.2.21
    lvgl/lvgl@^9.5.0
    adafruit/Adafruit ADS1X15@^2.6.2
    robtillaart/PCF8575@^0.3.0
build_flags = 
    -D LV_CONF_INCLUDE_SIMPLE
    -D LV_LVGL_H_INCLUDE_SIMPLE
    -I include
    -I lib
    -DPIO_FRAMEWORK_ARDUINO_ENABLE_FREERTOS
monitor_speed = 115200
```

### Build Troubleshooting Note (`MissingPackageManifestError`)
- **Symptom**: `MissingPackageManifestError: Could not find one of 'package.json' manifest files in the package` during `pio run`.
- **Cause**: Interrupted git clone in `~/.platformio/packages/framework-arduinopico`.
- **Fix**: Remove the corrupted folder with:
  ```powershell
  Remove-Item -Recurse -Force "$env:USERPROFILE\.platformio\packages\framework-arduinopico"
  pio run
  ```
- Detailed documentation: [`TROUBLESHOOTING_BUILD_ERROR.txt`](file:///e:/My%20Document/Projects/Custom%20Universal%20Remote%20Control/Universal_Controller_MainMCU_Code_pio/TROUBLESHOOTING_BUILD_ERROR.txt).

---

## 7. Known Hardware Quirks & Debug Notes

1. **Core Affinity Set Variable Reuse (`BackendTaskHandle.cpp`)**:
   - In `init_core0()`, `Core0_TaskHandle` is passed to three consecutive `xTaskCreate` calls (`task_UART_Handle`, `task_InputRead`, and `CoreData_Send`).
   - Consequently, `vTaskCoreAffinitySet(Core0_TaskHandle, 1<<0)` only applies affinity to `CoreData_Send`.
   - *Status*: Working because RP2040 FreeRTOS schedules tasks on available cores, but explicit handles per task are recommended for strict core pinning.
2. **SPI0 Bus Sharing (ST7789 & XPT2046)**:
   - Display and Touch share `SPI0` (`MOSI: 3`, `MISO: 4`, `SCLK: 6`).
   - LovyanGFX handles bus contention with separate Chip Select lines (`TFT_CS: 0` vs `TOUCH_CS: 7`) and clock speed adaptation (40 MHz TFT vs 2 MHz Touch). `bus_shared = true` is mandatory in `tft_lgfx_conf.h`.
3. **I2C Interrupt Line Pullup (`PCF_IRQ`)**:
   - PCF8575 open-drain INT pin requires `pinMode(PCF_IRQ, INPUT_PULLUP)`. An external pullup resistor is recommended to guarantee crisp falling edges at 400 kHz bus speeds.
4. **LVGL v9 Memory Allocation**:
   - `lv_conf.h` reserves `64 * 1024U` bytes for the internal LVGL pool (`LV_MEM_SIZE`).
   - `lv_display_buf` is aligned to 64-byte boundaries with size `320 * 240 / 4` (19.2 KB) for partial rendering, avoiding full-framebuffer RAM exhaustion.

---

## 8. Project Status & Changelog

### Verification Matrix

| Component / Subsystem | Verification Status | Notes |
| :--- | :--- | :--- |
| **Pico RP2040 Build Toolchain** | `[VERIFIED ON HARDWARE]` | Compiles cleanly; Earle Philhower core + FreeRTOS verified. |
| **LovyanGFX + ST7789 (SPI0)** | `[VERIFIED ON HARDWARE]` | Display init, rotation (3), and 40MHz write pipeline operational. |
| **XPT2046 Touch (SPI0)** | `[VERIFIED ON HARDWARE]` | Touch calibration active (`calib_data` loaded, 2MHz clock). |
| **LVGL 9.5.0 + EEZ-Studio GUI** | `[VERIFIED ON HARDWARE]` | Screens create properly; partial rendering buffer stable. |
| **ADS1015 Dual I2C ADC** | `[VERIFIED ON HARDWARE]` | Left (0x48) & Right (0x49) joystick sampling & center calibration active. |
| **PCF8575 Button Matrix (IRQ)** | `[VERIFIED ON HARDWARE]` | 10 buttons mapped to 16-bit expander with falling edge interrupt. |
| **FreeRTOS Dual-Core Mutex** | `[VERIFIED ON HARDWARE]` | `Core_Data_Mutex` passes telemetry between Core 0 and Core 1. |
| **Custom UART to Secondary MCU** | `[COMPILED - PENDING HARDWARE TEST]` | Framing implemented for `ESP_FC`, `BLE_CONTR`, and `IDLE`. |

### Changelog
- **2026-10-25**:
  - Resolved `MissingPackageManifestError` by clearing damaged package clone.
  - Successfully built `.uf2`, `.elf`, and `.bin` binaries.
  - Created [`TROUBLESHOOTING_BUILD_ERROR.txt`](file:///e:/My%20Document/Projects/Custom%20Universal%20Remote%20Control/Universal_Controller_MainMCU_Code_pio/TROUBLESHOOTING_BUILD_ERROR.txt).
- **2026-11-14**:
  - Re-read all source and configuration files across the codebase.
  - Authored comprehensive root [`GEMINI.md`](file:///e:/My%20Document/Projects/Custom%20Universal%20Remote%20Control/Universal_Controller_MainMCU_Code_pio/GEMINI.md) context, rules, pinout mappings, and hardware verification lifecycle.
