# ESP8266 Wireless Pager

A compact, wireless pager built using the ESP8266, designed to receive messages via Pushbullet, display them on an OLED screen, and offer additional features like vibration alerts, OTA updates, and a message vault.

## 🔧 Features

- 📩 **Message Notifications**  
  Receives messages through the Pushbullet API and displays them on-screen with an icon indicator and optional vibration feedback.

- 📟 **OLED Display UI**  
  A 1.3" SH1106 OLED screen displays a clean, user-friendly interface with:
  - Splash screen animation
  - Home screen with animated beating heart (or neutral icon)
  - Scrollable message vault

- 📳 **Vibration Alerts**  
  Discreet haptic feedback on message arrival via a vibration motor.

- 🔐 **Message Vault**  
  Stores predefined or received messages locally and allows navigation using a hardware button.

- 📱 **Find My Phone**  
  A dedicated button sends a signal via Pushbullet to trigger a "Find My Phone" alert on your paired device.

- 📡 **OTA Firmware Updates**  
  Easily push updates via HTTP using Arduino OTA mechanisms.

## 📲 Setup Instructions

1. **Install Prerequisites**
   - [Arduino IDE](https://www.arduino.cc/en/software)
   - ESP8266 board support via Boards Manager:
     ```
     https://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Required libraries:
     - `ESP8266WiFi`
     - `ESP8266HTTPClient`
     - `ArduinoJson`
     - `Adafruit_GFX`
     - `SH1106` or `U8g2`

2. **Configure `main.ino`**
   - Replace placeholders:
     ```cpp
     const char* ssid = "YOUR_SSID";
     const char* password = "YOUR_WIFI_PASSWORD";
     const char* pushbulletToken = "YOUR_ACCESS_TOKEN";
     const char* otaUrl = "https://yourdomain.com/update.bin";
     ```

3. **Upload & Test**
   - Connect ESP8266 via USB
   - Select correct board and port
   - Upload and monitor via Serial Monitor (115200 baud)

## 🖲️ Hardware Used

- ESP8266 (e.g., NodeMCU or Wemos D1 Mini)
- 1.3" SH1106 OLED Display (I2C)
- Vibration Motor (transistor-driven)
- 2x Pushbuttons
- Optional: Rechargeable LiPo with TP4056 module

## 🔄 OTA Update Instructions

Upload a `.bin` firmware file to a public server or GitHub and specify its URL in the firmware. On boot, the device will check for updates and install automatically.

```cpp
ESPhttpUpdate.update("https://yourdomain.com/update.bin");
