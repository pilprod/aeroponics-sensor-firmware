#include <Wire.h>
#include <iarduino_I2C_pH.h>
#include <iarduino_I2C_Relay.h>

iarduino_I2C_pH sensor(0x20); // pH-датчик
iarduino_I2C_Relay pwrkey(0x21); // Реле

void setup() {
    Serial.begin(9600); // Инициализация UART для отладки
    sensor.begin(&Wire); // Инициализация pH-датчика
    pwrkey.begin(); // Инициализация реле

    // Выключаем все каналы при старте
    pwrkey.digitalWrite(ALL_CHANNEL, HIGH);
}

void loop() {
    float pH = sensor.getPH();
    Serial.println(pH);
    // Проверяем наличие данных в Serial1 для управления реле
    if (Serial.available() > 0) {
        char command = Serial.read(); // Читаем команду из UART
        controlRelay(command); // Управляем реле в зависимости от команды
    }
    delay(3000);
    // Можно добавить дополнительный код для работы с pH-датчиком, если это необходимо
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
