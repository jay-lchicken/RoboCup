# RoboCup Robot — Repository Overview

## What This Project Is

This is the firmware for a **RoboCup Rescue Line** robot built on the **EVO X1** board (ESP32-S3). The robot follows a black line on a white field, detects coloured tiles (red, green, silver), avoids walls, navigates ramps, and enters an evacuation zone when a silver tile is detected.

## Hardware Platform

- **MCU**: ESP32-S3 (`esp32-s3-devkitm-1`)
- **Framework**: Arduino (via PlatformIO)
- **Board library**: [EVO-arduino](https://github.com/ljk1331ljk/EVO-arduino.git)

### Sensors & Actuators

| Component | Object | I2C Channel |
|---|---|---|
| Left RGB colour sensor | `csleft` | I2C8 |
| Right RGB colour sensor | `csright` | I2C7 |
| Front TOF distance sensor | `frontds` | I2C4 |
| Right TOF distance sensor | `rightds` | I2C5 |
| Left TOF distance sensor | `leftds` | I2C6 |
| IMU (gyroscope / pitch) | `gyro` | I2C1 |
| Line leader (analogue) | `lineLeader` | I2C3 |
| Left drive motor | `leftMotor` | M2 |
| Right drive motor | `rightMotor` | M1 |
| Motor pair helper | `motors` | — |
| Tri-button | `button` | GPIO1 |
| EVOX1 main board | `evo` | — |

## Build & Flash

```bash
# Build
pio run

# Flash over USB
pio run --target upload

# Flash over OTA (env:OTA — connects to EvoX1-OTA.local)
pio run -e OTA --target upload
```

PlatformIO configuration is in `platformio.ini`.

## Source File Map

| File | Purpose |
|---|---|
| `src/main.cpp` | Entry point — `setup()` runs the full match logic; `loop()` handles restart button |
| `src/definitions.h` | Global sensor/motor objects, calibration constants, `intialise()` |
| `src/getcolor.h` | HSV colour classification (`getColor`, `classifyColor`) |
| `src/linetrack.h` | PD line-following controller (`linetrack`) |
| `src/walltrack.h` | PD wall-following controllers (`walltrackleft`, `walltrackright`) |
| `src/linefinder.h` | Re-acquire the line after losing it (`findline`, `findlinesilver`) |

## Colour Encoding

```
0 = WHITE
1 = RED   (stop / end of run)
2 = GREEN (turn instruction)
3 = BLACK (line)
4 = SILVER (evacuation zone entrance)
```

## Key Calibration Constants (`definitions.h`)

| Constant | Default | Meaning |
|---|---|---|
| `linputmin` / `linputmax` | 80 / 336 | Raw clear-channel range for left colour sensor |
| `rinputmin` / `rinputmax` | 69 / 286 | Raw clear-channel range for right colour sensor |
| `outputMin` / `outputMax` | -20 / 300 | Mapped output range used in PD error calculations |

Colour HSV thresholds live in the `ColorThresholds` struct in `getcolor.h` and may need re-tuning for different lighting conditions.

## Main Robot Behaviour (`setup()`)

1. **Calibration mode** — if not woken by software reset, holds button 1 to display live sensor readings on the OLED for calibration.
2. **Wait for red** — holds until both colour sensors see red (start tile).
3. **Line-following loop** — runs until red is seen again (end of run):
   - **Ramp detection** — uses gyro pitch; slows and follows the line through ramps.
   - **Green tile** — turns right (left sensor), left (right sensor), or U-turn (both sensors).
   - **Silver tile** — calls `findlinesilver()`; if confirmed, enters evacuation-zone wall-following.
   - **Obstacle avoidance** — if the front TOF sensor reads < 90 mm, performs a back-and-turn manoeuvre then re-finds the line.
   - **Normal** — calls `linetrack(25, 30, 10, 3000, lc, rc)`.
4. **`loop()`** — polls button 2 to trigger a software restart (`esp_restart()`).
