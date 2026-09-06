#include <Wire.h>
#include <iarduino_I2C_pH.h>
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"
#include <Arduino.h>

// Прототипы функций
void connectToWiFi();
void connectToMQTT();
void publishData(float averagePH, float averageHumidity1, float averageHumidity2);
void resetDevice();

// Подключение к WiFi
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = MQTT_BROKER;
int port = 1883;
const char publishTopic[] PROGMEM = "arduino/ph";               // Тема для отправки данных pH
const char humidityTopic1[] PROGMEM = "arduino/substrate_sensor_S1";  // Тема для датчика влажности 1
const char humidityTopic2[] PROGMEM = "arduino/substrate_sensor_S2";  // Тема для датчика влажности 2

iarduino_I2C_pH sensor(0xE); // Используем pH-метр с адресом 0xB

// Константы для инверсного чтения влажности
#define MIN_HUMIDITY_5V 630  // Минимальное значение датчика влажности при питании 5V (в воздухе)
#define MAX_HUMIDITY_5V 330  // Максимальное значение датчика влажности при питании 5V (в воде)

#define MIN_HUMIDITY_3V 400  // Минимальное значение датчика влажности при питании 3.3V (в воздухе)
#define MAX_HUMIDITY_3V 200  // Максимальное значение датчика влажности при питании 3.3V (в воде)

// Буфер для хранения последних измерений pH и влажности
const int bufferSize = 10;
float phBuffer[bufferSize];
float humidityBuffer1[10]; // Буфер для значений влажности за 30 секунд (по 1 в секунду)
float humidityBuffer2[10]; // Буфер для второго датчика
int bufferIndex = 0;
int humidityBufferIndex = 0;
unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 10000; // Публикация каждые 30 секунд
unsigned long lastResetTime = 0;
const unsigned long resetInterval = 3600000; // Перезагрузка устройства каждый час (3600000 мс = 1 час)

// Дополнительные переменные для подсчета пропущенных публикаций
int publishCount = 0; // Счетчик количества публикаций

// Переменная для режима питания датчика влажности
bool use5V = true;  // true для 5V, false для 3.3V

void setup() {
    Serial.begin(9600);
    while (!Serial) {;}

    // Подключение к WiFi
    connectToWiFi();

    // Подключение к MQTT
    connectToMQTT();

    // Инициализация pH-метра
    sensor.begin(&Wire);

    // Инициализация буферов
    memset(phBuffer, 0, sizeof(phBuffer));  
    memset(humidityBuffer1, 0, sizeof(humidityBuffer1));  
    memset(humidityBuffer2, 0, sizeof(humidityBuffer2));  

    // Запоминаем время старта
    lastResetTime = millis();

    // Считывание текущего значения pH
    float phValue = sensor.getPH();

    // Считывание значений датчиков влажности с портов A0 и A1
    float humidity1 = analogRead(A0); // Датчик влажности на A0
    float humidity2 = analogRead(A1); // Датчик влажности на A1
}

void loop() {
    // Проверка и переподключение WiFi и MQTT при необходимости
    if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi();
    }

    if (!mqttClient.connected()) {
        connectToMQTT();
    }

    // Считывание текущего значения pH
    float phValue = sensor.getPH();

    // Добавляем значение в буфер
    phBuffer[bufferIndex] = phValue;
    bufferIndex = (bufferIndex + 1) % bufferSize;

    // Считывание значений датчиков влажности с портов A0 и A1
    float humidity1 = analogRead(A0); // Датчик влажности на A0
    float humidity2 = analogRead(A1); // Датчик влажности на A1

    // Преобразование аналоговых значений в проценты влажности в зависимости от режима питания
    if (use5V) {
        humidity1 = map(humidity1, MIN_HUMIDITY_5V, MAX_HUMIDITY_5V, 0, 100); // Преобразуем в % влажности (5V)
        humidity2 = map(humidity2, MIN_HUMIDITY_5V, MAX_HUMIDITY_5V, 0, 100); // Преобразуем в % влажности (5V)
    } else {
        humidity1 = map(humidity1, MIN_HUMIDITY_3V, MAX_HUMIDITY_3V, 0, 100); // Преобразуем в % влажности (3.3V)
        humidity2 = map(humidity2, MIN_HUMIDITY_3V, MAX_HUMIDITY_3V, 0, 100); // Преобразуем в % влажности (3.3V)
    }

    // Инвертирование значений
    humidity1 = 100 - humidity1;
    humidity2 = 100 - humidity2;

    // Сохраняем текущие показания влажности в буфер
    humidityBuffer1[humidityBufferIndex] = humidity1;
    humidityBuffer2[humidityBufferIndex] = humidity2;
    humidityBufferIndex = (humidityBufferIndex + 1) % 10;  // Циклический буфер на 30 значений (1 значение в секунду)

    // Проверяем, пора ли публиковать данные (раз в 30 секунд)
    unsigned long currentTime = millis();
    if (currentTime - lastPublishTime >= publishInterval) {
        // Рассчитываем среднее значение pH за последние измерения
        float sumPH = 0.0;
        for (int i = 0; i < bufferSize; i++) {
            sumPH += phBuffer[i];
        }
        float averagePH = sumPH / bufferSize;

        // Рассчитываем средние значения влажности за последние 30 секунд
        float sumHumidity1 = 0.0;
        float sumHumidity2 = 0.0;
        for (int i = 0; i < 10; i++) {
            sumHumidity1 += humidityBuffer1[i];
            sumHumidity2 += humidityBuffer2[i];
        }
        float averageHumidity1 = sumHumidity1 / 10;
        float averageHumidity2 = sumHumidity2 / 10;

        // Увеличиваем счетчик публикаций
        publishCount++;

        // Публикуем данные, если было больше 6 публикаций
        if (publishCount > 6) {
            publishData(averagePH, averageHumidity1, averageHumidity2);
        }

        // Очищаем буфер после публикации
        memset(phBuffer, 0, sizeof(phBuffer));
        memset(humidityBuffer1, 0, sizeof(humidityBuffer1));
        memset(humidityBuffer2, 0, sizeof(humidityBuffer2));

        // Обновляем время последней публикации
        lastPublishTime = currentTime;
    }

    // Проверяем, прошел ли час для перезагрузки устройства
    if (currentTime - lastResetTime >= resetInterval) {
        Serial.println(F("Прошла 1 час. Перезагрузка устройства..."));
        NVIC_SystemReset();  // Перезагрузка устройства
    }

    delay(1000); // Задержка для получения новых данных каждую секунду
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

// Функция для отправки данных pH и влажности
void publishData(float averagePH, float averageHumidity1, float averageHumidity2) {
    // Отправка среднего значения pH
    if (mqttClient.beginMessage(publishTopic, true, 1)) {  // retain = true, QoS = 1
        mqttClient.print(F("{\"pH\": "));
        mqttClient.print(averagePH, 2);
        mqttClient.print(F("}"));
        mqttClient.endMessage();
    }
    Serial.print(F("Среднее значение pH отправлено: "));
    Serial.println(averagePH, 2);

    // Отправка среднего значения влажности с первого датчика
    if (mqttClient.beginMessage(humidityTopic1, true, 1)) {  // retain = true, QoS = 1
        mqttClient.print(F("{\"humidity\": "));
        mqttClient.print(averageHumidity1, 2);
        mqttClient.print(F("}"));
        mqttClient.endMessage();
    }
    Serial.print(F("Среднее значение влажности (датчик 1) отправлено: "));
    Serial.println(averageHumidity1, 2);

    // Отправка среднего значения влажности со второго датчика
    if (mqttClient.beginMessage(humidityTopic2, true, 1)) {  // retain = true, QoS = 1
        mqttClient.print(F("{\"humidity\": "));
        mqttClient.print(averageHumidity2, 2);
        mqttClient.print(F("}"));
        mqttClient.endMessage();
    }
    Serial.print(F("Среднее значение влажности (датчик 2) отправлено: "));
    Serial.println(averageHumidity2, 2);
}
