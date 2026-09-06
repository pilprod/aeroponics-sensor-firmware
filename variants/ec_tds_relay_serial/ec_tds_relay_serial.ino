#include <Wire.h>
#include <iarduino_I2C_TDS.h>
#include <iarduino_I2C_pH.h>
#include <iarduino_I2C_Relay.h>
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

void setupTDSSensor();

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

iarduino_I2C_TDS tds(0x10);
// iarduino_I2C_pH sensor(0x20);
iarduino_I2C_Relay pwrkey(0x21);

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = MQTT_BROKER;
int port = 1883;

void setup() {
    Serial.begin(9600); // Инициализируем UART
    Serial1.begin(9600); // Инициализируем UART для передачи команд
    // sensor.begin(&Wire); // Инициализация pH-датчика
    tds.begin(&Wire);
    pwrkey.begin(); // Инициализируем реле

    // setupTDSSensor();

    // Подключение к WiFi и MQTT
    connectToWiFi();
    connectToMQTT();
}

void loop() {
    float ecValue = tds.getEC();
    Serial.print("Значение EC: ");
    Serial.println(ecValue);
    float tdsValue = tds.getTDS();
    Serial.print("Значение TDS: ");
    Serial.println(tdsValue);
    // Проверяем наличие данных в Serial1 для управления реле
    if (Serial1.available() > 0) {
        char command = Serial1.read(); // Читаем команду из UART
        controlRelay(command); // Управляем реле в зависимости от команды
        float receivedPH = Serial1.parseFloat();  // Читаем переданное значение pH.
        Serial.print("Полученное значение pH: ");
        Serial.println(receivedPH);              // Выводим значение в монитор.
    }
    delay(3000); // Задержка
}

// Функция для управления реле
void controlRelay(char command) {
    switch (command) {
        case '1': // Включаем первый канал
            pwrkey.digitalWrite(1, HIGH);
            break;
        case '2': // Выключаем первый канал
            pwrkey.digitalWrite(1, LOW);
            break;
        case '3': // Включаем второй канал
            pwrkey.digitalWrite(2, HIGH);
            break;
        case '4': // Выключаем второй канал
            pwrkey.digitalWrite(2, LOW);
            break;
        case '5': // Включаем третий канал
            pwrkey.digitalWrite(3, HIGH);
            break;
        case '6': // Выключаем третий канал
            pwrkey.digitalWrite(3, LOW);
            break;
        case '7': // Включаем четвертый канал
            pwrkey.digitalWrite(4, HIGH);
            break;
        case '8': // Выключаем четвертый канал
            pwrkey.digitalWrite(4, LOW);
            break;
        case '0': // Выключаем все каналы
            pwrkey.digitalWrite(ALL_CHANNEL, LOW);
            break;
    }
}

// Подключение к WiFi
void connectToWiFi() {
    Serial.print(F("Подключение к сети WiFi "));
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);
        Serial.print(F("."));
        delay(5000);
    }

    Serial.println(F("Подключено к сети WiFi"));
}

// Подключение к MQTT
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
