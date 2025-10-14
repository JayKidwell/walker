
# Copilot Instructions for walker (Python Source)

## Project Overview
- This repository controls a multi-servo walking robot using Python and the Adafruit ServoKit library.
- The main logic is in `source/sample1.py`, which demonstrates servo coordination for a 4-legged robot (12 servos).

## Architecture & Key Components
- **Servo Control**: Uses `adafruit_servokit.ServoKit` for 16-channel PWM servo control.
- **Leg/Joint Abstraction**: Each leg has 3 joints (finger, elbow, shoulder). Angles for all servos are managed via lists (e.g., `POS_Flat`, `POS_STAND_A`).
- **Movement Patterns**: Functions like `layFlat`, `stand`, and `wiggle` set servo positions for coordinated movement.
- **Logging**: Uses Python `logging` for debug output, with log level set via the `LOG_LEVEL` environment variable.

## Developer Workflows
- **Run the Robot Demo**: Execute `python source/sample1.py` to run the main walking/servo test routine.
- **Modify Movement**: Edit position lists (`POS_Flat`, `POS_STAND_A`, etc.) or movement functions to change robot behavior.
- **Debugging**: Adjust `LOG_LEVEL` (e.g., `export LOG_LEVEL=DEBUG`) for more verbose output.
- **Dependencies**: Install `adafruit-circuitpython-servokit` via pip: `pip install adafruit-circuitpython-servokit`.

## Conventions & Patterns
- **Servo Indexing**: Servos are indexed by channel (0-15). Each leg uses 3 consecutive channels.
- **Function Structure**: Movement routines are modular (`wiggle`, `layFlat`, `stand`). Use helper functions for joint/leg/servo angle setting.
- **No External Configs**: All movement parameters are hardcoded in `sample1.py` for clarity and direct modification.

## Integration Points
- **Hardware**: Designed for Adafruit PCA9685 PWM controller and compatible servos.
- **Environment**: Requires Python 3 and the Adafruit ServoKit library.

## Tips for AI Agents
- Focus on `source/sample1.py` for all logic and conventions.
- When adding new movement patterns, follow the modular function style and update position lists as needed.
- Use logging for debugging and traceability.
- Ensure hardware dependencies are documented for new features.

---
Feedback welcome! If any section is unclear or missing, please specify so it can be improved.