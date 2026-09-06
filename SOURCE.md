# Source provenance

- Private source repository: https://gitlab.com/leafcoin/legacy/arduino.git
- Snapshot commit: `6ded2cb40908ba4feca6ff575d1a826cf08b4f08`
- Selected source path: `uno/uno.ino`

The archived repository's commits identify Ilya Popov (Ilya Papou) as author. It records personal R&D during 2024.

## Snapshot changes

The sketch is copied without redesigning its control flow. Its local MQTT broker address is replaced by the `MQTT_BROKER` configuration macro. A placeholder secrets-header example, ignore rules and documentation are newly added for this public-facing snapshot.

Earlier prototypes, backups, generated settings, local upload-device identifiers and the unreviewed diagram are omitted. No source commit history is transferred. The original private GitLab repository remains intact.

## Dependencies and rights

The sketch references Arduino networking/JSON libraries, `iarduino_I2C_Software` and a modified Adafruit TCS34725 library. These dependencies are not bundled; their respective upstream licenses apply separately. The modified library is missing from the archived source and has not been reconstructed here.

No LICENSE file was present in the source repository. This snapshot adds no license grant. Manufacturer/library names describe compatibility and dependencies, not affiliation or endorsement.
