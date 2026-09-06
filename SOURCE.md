# Source provenance

- Private source repository: https://gitlab.com/leafcoin/legacy/arduino.git

The archived repository's commits identify Ilya Popov (Ilya Papou) as author. It records personal R&D during 2024.

## Selected prototypes

Each destination is an independent Arduino sketch directory. Dates are source-revision dates; the snapshots are prototypes, not releases or a completed control system.

| Destination | Original source path | Exact source commit | Date |
| --- | --- | --- | --- |
| `uno/uno.ino` | `uno/uno.ino` | `6ded2cb40908ba4feca6ff575d1a826cf08b4f08` | 2024-11-10 |
| `variants/tcs34725_serial/tcs34725_serial.ino` | `pirania/pirania.ino` | `6ded2cb40908ba4feca6ff575d1a826cf08b4f08` | 2024-11-10 |
| `variants/ph_relay_serial/ph_relay_serial.ino` | `pirania/pirania.ino` | `de2bcdfcca58b0cae0acd434ac28f2b5fc9a2d27` | 2024-10-28 |
| `variants/ec_tds_relay_serial/ec_tds_relay_serial.ino` | `uno/arduino.ino` | `de2bcdfcca58b0cae0acd434ac28f2b5fc9a2d27` | 2024-10-28 |
| `variants/ph_moisture_mqtt/ph_moisture_mqtt.ino` | `arduino.ino` | `2ff2b8a770571c0a891b07d7c5cc851adb14a813` | 2024-09-29 |

## Snapshot changes

The sketches are copied without redesigning control flow, calibration constants, timing, relay states or command handling. The two Serial-only variants retain the source text unchanged apart from a final newline where needed.

In `uno/uno.ino`, `variants/ec_tds_relay_serial/ec_tds_relay_serial.ino` and `variants/ph_moisture_mqtt/ph_moisture_mqtt.ino`, the local broker address is replaced by the `MQTT_BROKER` configuration macro. Their existing Wi-Fi configuration remains based on `SECRET_SSID` and `SECRET_PASS`. A placeholder `arduino_secrets.example.h` is provided beside each networked sketch; real `arduino_secrets.h` files are ignored and are not included. No authentication, TLS or new runtime behavior is implemented by these configuration changes.

The four additional sketches are placed in separate directories with matching sketch filenames to avoid combining their independent `setup()` and `loop()` functions. Documentation and the shared lab photo gallery are added for context. Five photographs disclose AI background/privacy retouching. Photographs do not validate the archived code.

Backups, generated settings, local upload-device identifiers, commented-only utility examples and the unreviewed editable diagram are omitted. The gallery's wiring-diagram photograph is separate from that omitted diagram file. No source commit history is transferred. The original private GitLab repository remains intact.

## Dependencies and rights

The sketches reference Arduino networking/JSON libraries, `SoftwareWire`, iarduino pH/TDS/relay/software-I2C libraries and modified Adafruit TCS34725 libraries. These dependencies are not bundled; their respective upstream licenses apply separately. The modified `Adafruit_TCS34725softi2c` and `Adafruit_TCS34725_Soft.h` implementations are missing from the archived source and have not been reconstructed here. Commit attribution describes the user's archived integration work, not ownership of the referenced upstream libraries or commercial hardware.

No LICENSE file was present in the source repository. This snapshot adds no license grant. Manufacturer/library names describe compatibility and dependencies, not affiliation or endorsement.
