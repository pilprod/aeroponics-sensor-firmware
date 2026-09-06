// #include <SoftwareWire.h>
#include <iarduino_I2C_Software.h>          // Подключаем библиотеку iarduino_I2C_Software
#include <Adafruit_TCS34725softi2c.h>
// #include <Adafruit_CCS811.h>
#include <ArduinoJson.h>
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

// Настройки WiFi и MQTT
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char broker[] = MQTT_BROKER;
const int port = 1883;
const int qos = 1;
const bool retain = true;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

#define INTEGRATION_TIME TCS34725_INTEGRATIONTIME_700MS
#define GAIN_SETTING TCS34725_GAIN_1X

// Период проверки доступности датчиков (30 секунд)
const unsigned long CHECK_INTERVAL = 30000;
unsigned long lastCheckTime = 0;

#define SDA1 2
#define SCL1 3
#define SDA2 4
#define SCL2 5
#define SDA3 6
#define SCL3 7
#define SDA4 8
#define SCL4 9
// #define SDA_CCS1 10
// #define SCL_CCS1 11
// #define SDA_CCS2 12
// #define SCL_CCS2 13

// Экземпляры SoftwareWire для I2C-шин
// SoftwareWire myWire1(SDA1, SCL1);
// SoftwareWire myWire2(SDA2, SCL2);
// SoftwareWire myWire3(SDA3, SCL3);
// SoftwareWire myWire4(SDA4, SCL4);
// SoftwareWire myWireCCS1(SDA_CCS1, SCL_CCS1);
// SoftwareWire myWireCCS2(SDA_CCS2, SCL_CCS2);

Adafruit_TCS34725softi2c tcs1(INTEGRATION_TIME, GAIN_SETTING, SDA1, SCL1);
Adafruit_TCS34725softi2c tcs2(INTEGRATION_TIME, GAIN_SETTING, SDA2, SCL2);
Adafruit_TCS34725softi2c tcs3(INTEGRATION_TIME, GAIN_SETTING, SDA3, SCL3);
Adafruit_TCS34725softi2c tcs4(INTEGRATION_TIME, GAIN_SETTING, SDA4, SCL4);

// Adafruit_CCS811 ccs1(&myWireCCS1);
// Adafruit_CCS811 ccs2(&myWireCCS2);

bool tcsOnline[] = {false, false, false, false};
// bool ccsOnline[] = {false, false};
String tcsTopics[] = {"tcs1", "tcs2", "tcs3", "tcs4"};
// String ccsTopics[] = {"ccs1", "ccs2"};
Adafruit_TCS34725softi2c* tcsSensors[] = {&tcs1, &tcs2, &tcs3, &tcs4};
// Adafruit_CCS811* ccsSensors[] = {&ccs1, &ccs2};

void setup() {
  Serial.begin(9600);
  delay(1000);

  connectToWiFi();
  connectToMQTT();

  myWire1.begin();
  myWire2.begin();
  myWire3.begin();
  myWire4.begin();
  // myWireCCS1.begin();
  // myWireCCS2.begin();

  // Инициализация датчиков
  for (int i = 0; i < 4; i++) {
    tcsOnline[i] = initializeTCS34725(*tcsSensors[i], tcsTopics[i]);
  }
  // for (int i = 0; i < 2; i++) {
  //   ccsOnline[i] = initializeCCS811(*ccsSensors[i], ccsTopics[i]);
  // }

  // Подписка на MQTT топики
  for (int i = 0; i < 4; i++) {
    mqttClient.subscribe("arduino/light_sensors/" + tcsTopics[i] + "/#");
  }
  // for (int i = 0; i < 2; i++) {
  //   mqttClient.subscribe("arduino/air_quality/" + ccsTopics[i] + "/#");
  // }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.poll();

  unsigned long currentTime = millis();
  if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
    lastCheckTime = currentTime;

    for (int i = 0; i < 4; i++) {
      tcsOnline[i] = initializeTCS34725(*tcsSensors[i], tcsTopics[i]);
    }
    // for (int i = 0; i < 2; i++) {
    //   ccsOnline[i] = initializeCCS811(*ccsSensors[i], ccsTopics[i]);
    // }
  }

  for (int i = 0; i < 4; i++) {
    readAndProcessTCS34725(*tcsSensors[i], tcsTopics[i], tcsOnline[i]);
  }
  // for (int i = 0; i < 2; i++) {
  //   readAndProcessCCS811(*ccsSensors[i], ccsTopics[i], ccsOnline[i]);
  // }

  delay(1000);
}

void connectToWiFi() {
  Serial.print(F("Подключение к сети WiFi "));
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("."));
    delay(5000);
  }
  Serial.println(F("Подключено к сети WiFi"));
}

void connectToMQTT() {
  Serial.print(F("Подключение к MQTT брокеру: "));
  Serial.println(broker);

  while (!mqttClient.connect(broker, port)) {
    Serial.print(F("Ошибка подключения к MQTT. Код ошибки: "));
    Serial.println(mqttClient.connectError());
    delay(5000);
  }
  Serial.println(F("Подключено к MQTT брокеру!"));
}

void reconnectMQTT() {
  Serial.print(F("Попытка переподключения к MQTT брокеру..."));
  while (!mqttClient.connect(broker, port)) {
    Serial.print(F("."));
    delay(5000);
  }
  Serial.println(F("Подключено к MQTT брокеру!"));
}

bool initializeTCS34725(Adafruit_TCS34725softi2c &tcs, const String& sensorName) {
  if (tcs.begin()) {
    Serial.print(sensorName);
    Serial.println(" обнаружен.");
    return true;
  } else {
    Serial.print(sensorName);
    Serial.println(" не обнаружен.");
    return false;
  }
}

// bool initializeCCS811(Adafruit_CCS811 &ccs, const String& sensorName) {
//   if (ccs.begin()) {
//     Serial.print(sensorName);
//     Serial.println(" обнаружен.");
//     return true;
//   } else {
//     Serial.print(sensorName);
//     Serial.println(" не обнаружен.");
//     return false;
//   }
// }

void readAndProcessTCS34725(Adafruit_TCS34725softi2c &tcs, const String& sensorName, bool online) {
  uint16_t clear, red, green, blue;
  if (online) {
    tcs.getRawData(&red, &green, &blue, &clear);
    float colorTemp = tcs.calculateColorTemperature(red, green, blue);
    float lux = tcs.calculateLux(red, green, blue);

    StaticJsonDocument<200> doc;
    doc["sensor"] = sensorName;
    doc["online"] = online;
    doc["red"] = red;
    doc["green"] = green;
    doc["blue"] = blue;
    doc["clear"] = clear;
    doc["colorTemp"] = colorTemp;
    doc["lux"] = lux;

    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer); // Теперь длину мы не указываем
    String topic = "arduino/light_sensors/" + sensorName;
    mqttClient.beginMessage(topic.c_str());  // Начинаем публикацию сообщения
    mqttClient.print(jsonBuffer);            // Печатаем JSON-пакет
    mqttClient.endMessage();                 // Завершаем публикацию

    Serial.print(sensorName);
    Serial.println(" данные отправлены в MQTT.");
  } else {
    Serial.print("Ошибка чтения ");
    Serial.println(sensorName);
  }
}

// void readAndProcessCCS811(Adafruit_CCS811 &ccs, const String& sensorName, bool online) {
//   if (online && ccs.available()) {
//     if (!ccs.readData()) {
//       int eCO2 = ccs.geteCO2();
//       int TVOC = ccs.getTVOC();

//       StaticJsonDocument<200> doc;
//       doc["sensor"] = sensorName;
//       doc["online"] = online;
//       doc["eCO2"] = eCO2;
//       doc["TVOC"] = TVOC;

//       char jsonBuffer[256];
//       size_t n = serializeJson(doc, jsonBuffer);
//       String topic = "arduino/air_quality/" + sensorName;
//       mqttClient.publish(topic.c_str(), jsonBuffer, n, retain, qos);

//       Serial.print(sensorName);
//       Serial.print(" - eCO2: "); Serial.print(eCO2);
//       Serial.print(" ppm, TVOC: "); Serial.print(TVOC);
//       Serial.println(" ppb данные отправлены в MQTT.");
//     } else {
//       Serial.print("Ошибка чтения ");
//       Serial.println(sensorName);
//     }
//   }
// }

// Обработчик входящих сообщений MQTT
void messageReceived(int messageSize) {
  String topic = mqttClient.messageTopic();
  String payload = mqttClient.readString();

  for (int i = 0; i < 4; i++) {
    if (tcsOnline[i]) {
      if (topic == "arduino/light_sensors/" + tcsTopics[i] + "/setIntegrationTime") {
        tcsSensors[i]->setIntegrationTime(static_cast<tcs34725IntegrationTime_t>(payload.toInt()));
      } else if (topic == "arduino/light_sensors/" + tcsTopics[i] + "/setGain") {
        tcsSensors[i]->setGain(static_cast<tcs34725Gain_t>(payload.toInt()));
      } else if (topic == "arduino/light_sensors/" + tcsTopics[i] + "/setInterrupt") {
        tcsSensors[i]->setInterrupt(payload == "true");
      } else if (topic == "arduino/light_sensors/" + tcsTopics[i] + "/setIntLimits") {
        int lowLimit = payload.substring(0, payload.indexOf(',')).toInt();
        int highLimit = payload.substring(payload.indexOf(',') + 1).toInt();
        tcsSensors[i]->setIntLimits(lowLimit, highLimit);
      }
    }
  }
}
