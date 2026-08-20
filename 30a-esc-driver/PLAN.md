# 30A ESC Driver - Project Plan

ESP32-based controller for a 30A Opto BLDC ESC motor controller with a web-based control panel.

## Goal

Build an ESP32 firmware that:
1. Drives a 30A Opto BLDC ESC via a standard servo-style PWM signal
2. Hosts an HTML web server with a control panel UI
3. Provides real-time motor speed control through a browser

## Hardware

| Component | Description |
|-----------|-------------|
| ESP32 Dev Board | Microcontroller (WiFi-capable) |
| 30A Opto BLDC ESC | Electronic Speed Controller for brushless DC motors |
| BLDC Motor | Brushless DC motor connected to the ESC |
| Power Supply | Battery or bench supply appropriate for the motor |

### Wiring

- **ESC signal wire** connects to an ESP32 GPIO pin (e.g., GPIO 13)
- **ESC ground** connects to ESP32 ground (shared ground is required)
- **ESC power wires** connect to the motor and battery (not through the ESP32)
- The "Opto" designation means the ESC optically isolates the signal input -- it does **not** provide 5V BEC power on the signal connector, so the ESP32 must be powered independently (USB or separate regulator)

## How a BLDC ESC Works

A standard BLDC ESC accepts a servo-style PWM signal:
- **Frequency**: 50 Hz (20 ms period), same as a standard RC servo
- **Pulse width range**: 1000 us (off/minimum) to 2000 us (full throttle)
- **Arming sequence**: Most ESCs require a low-throttle signal (1000 us) for a few seconds at power-on before they will accept throttle commands. This prevents accidental motor spin-up.

The ESP32 `Servo` library (or `ESP32Servo` on PlatformIO) maps an angle value (0-180) to the 1000-2000 us pulse range, which is exactly what we need.

## Development Environment

- **IDE**: VS Code with PlatformIO extension
- **Framework**: Arduino (via PlatformIO for ESP32)
- **Language**: C++
- **Board**: `esp32dev` (generic ESP32 dev module)

## Project Structure

```
30a-esc-driver/
  platformio.ini          # PlatformIO project configuration
  PLAN.md                 # This plan document
  src/
    main.cpp              # Entry point: setup(), loop()
  data/
    index.html            # Web UI (uploaded to SPIFFS)
```

## Phase 1 - Features

### 1. ESC PWM Control
- Use the `ESP32Servo` library to generate a 50 Hz PWM signal
- Map speed values (0-100%) to pulse widths (1000-2000 us)
- Implement arming sequence on startup (hold 1000 us for ~3 seconds)

### 2. Web Server Control Panel
- ESP32 connects to WiFi (station mode with credentials configured in code)
- Hosts an async web server on port 80
- Serves an HTML page from SPIFFS with:
  - **Speed slider**: Range input from 0% to 100%, controls motor speed
  - **On/Off toggle button**: Immediately cuts motor to 0% (off) or restores to slider position (on)
- API endpoints:
  - `GET /` -- Serve the HTML control panel
  - `GET /speed?value=<0-100>` -- Set motor speed (returns JSON confirmation)
  - `GET /toggle?state=<on|off>` -- Toggle motor on/off (returns JSON confirmation)
  - `GET /status` -- Return current speed and on/off state as JSON

### 3. Web UI Design
- Vanilla HTML/CSS/JavaScript -- no frameworks (Bootstrap, jQuery, Angular, etc.)
- Single self-contained HTML file with inline styles and script
- Responsive layout that works on phone and desktop browsers
- Uses native `fetch()` API for server communication
- Slider sends speed updates as the user drags (debounced at ~100ms to avoid flooding)
- Toggle button visually reflects current state (green=on, red=off)
- Fetches `/status` on page load to sync with current motor state

## Implementation Steps

### Step 1: Scaffold PlatformIO Project
- Create `platformio.ini` targeting `esp32dev` with Arduino framework
- Add library dependencies: `ESP32Servo`, `ESPAsyncWebServer`, `AsyncTCP`
- Configure SPIFFS filesystem upload
- Add WiFi credentials as build flags or constants

### Step 2: Implement ESC Control (main.cpp)
- Initialize `Servo` object on the ESC signal pin
- Write `armEsc()` function that sends 1000 us for 3 seconds
- Write `setSpeed(int percent)` function that maps 0-100 to 1000-2000 us
- Call `armEsc()` in `setup()`
- Add global state variables: `currentSpeed` (int 0-100), `motorEnabled` (bool)

### Step 3: Implement WiFi and Web Server (main.cpp)
- Connect to WiFi in `setup()`, print IP address to Serial
- Create `AsyncWebServer` on port 80
- Register route handlers:
  - `GET /` serves `index.html` from SPIFFS
  - `GET /speed?value=N` updates `currentSpeed` and applies if `motorEnabled`
  - `GET /toggle?state=on|off` sets `motorEnabled`, applies 0 or `currentSpeed`
  - `GET /status` returns JSON `{"speed": N, "enabled": true/false}`
- Start the server

### Step 4: Build the Web UI (data/index.html)
- HTML page with:
  - Title/header: "ESC Motor Control"
  - Range slider: min=0, max=100, step=1, with percentage label
  - Toggle button: styled as on/off switch
  - Status indicator showing current values
- JavaScript:
  - On slider `input` event, send fetch to `/speed?value=N` (debounced ~100ms)
  - On button click, send fetch to `/toggle?state=on|off`
  - On page load, fetch `/status` to initialize UI state
- CSS:
  - Clean, centered layout
  - Large slider and button for easy touch control
  - Color-coded toggle (green/red)

### Step 5: Upload and Test
- Build and upload firmware via PlatformIO
- Upload SPIFFS filesystem image (for index.html)
- Monitor serial output for WiFi IP address
- Open browser to the IP address and test controls
- Verify ESC arms correctly and motor responds to slider

## Dependencies (PlatformIO Libraries)

| Library | Purpose |
|---------|---------|
| `ESP32Servo` | Servo/PWM signal generation for ESC control |
| `ESPAsyncWebServer` | Async HTTP web server |
| `AsyncTCP` | TCP layer required by ESPAsyncWebServer |

## Configuration Constants

```cpp
// WiFi
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";

// ESC
const int ESC_PIN = 13;           // GPIO pin for ESC signal
const int ESC_MIN_US = 1000;      // Minimum pulse width (off)
const int ESC_MAX_US = 2000;      // Maximum pulse width (full throttle)
const int ESC_ARM_DELAY_MS = 3000; // Arming duration

// Server
const int SERVER_PORT = 80;
```

## Safety Considerations

- **Arming**: ESC will not accept throttle until the arming sequence completes
- **Default off**: Motor starts in the disabled state after boot
- **Kill switch**: The on/off toggle immediately sets throttle to minimum (1000 us)
- **WiFi disconnect**: If WiFi drops, the motor maintains its last commanded state -- a future enhancement could add a watchdog that cuts throttle if no commands are received within a timeout period
- **No reverse**: Standard ESC firmware is forward-only; the slider range is 0-100% forward throttle

## Future Enhancements (Out of Scope for Phase 1)

- mDNS so the control panel is accessible via `http://esc.local` instead of IP
- OTA (over-the-air) firmware updates
- Watchdog timer to cut motor on WiFi/command timeout
- Telemetry display (ESC temperature, current draw if supported)
- Multiple motor support
- Ramp-up/ramp-down acceleration curves
- Access point (AP) fallback mode if station WiFi fails to connect
- Save last-used settings to EEPROM/NVS
