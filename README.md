# Aeroponics sensor firmware — historical R&D snapshot

An Arduino Uno R4 WiFi light-sensor prototype from a personal aeroponics lab. It combines four TCS34725 sensors, RGB/clear readings, derived lux and color temperature, JSON telemetry and MQTT publication. This is an archival source snapshot, not a currently buildable firmware release.

## Included code

`uno/uno.ino` contains sensor initialization, periodic availability checks, Wi-Fi/MQTT reconnection, JSON telemetry and a command handler for gain, integration time and interrupt settings.

Dependencies are `WiFiS3`, `ArduinoMqttClient`, `ArduinoJson`, `iarduino_I2C_Software` and a modified `Adafruit_TCS34725softi2c` library. External libraries are not bundled.

## Configuration

Copy `uno/arduino_secrets.example.h` to the ignored `uno/arduino_secrets.h` and provide local Wi-Fi and MQTT settings there. Do not commit real credentials. Review the limitations below before attempting compilation or connecting hardware.

## Known archival limitations

- Calls to `myWire1`–`myWire4` remain while their declarations are commented out.
- The modified sensor-library dependency is missing from the archived source.
- `messageReceived` is defined but not registered as an MQTT callback.
- Commented-out CCS811 code is not an implemented feature.

These issues are preserved rather than silently repaired. Compilation, hardware operation, calibration and safety have not been tested for this snapshot. MQTT transport/security and command handling require review before real use.

See [SOURCE.md](SOURCE.md) for provenance. The original repository is retained; no new license grant is added.
