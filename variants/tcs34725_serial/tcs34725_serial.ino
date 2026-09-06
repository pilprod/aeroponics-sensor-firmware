// #include <Wire.h>
// #include <iarduino_I2C_pH.h>
// #include <iarduino_I2C_Relay.h>

// iarduino_I2C_pH sensor(0x20); // pH-датчик
// iarduino_I2C_Relay pwrkey(0x21); // Реле

// void setup() {
//     Serial.begin(9600); // Инициализация UART для отладки
//     sensor.begin(&Wire); // Инициализация pH-датчика
//     pwrkey.begin(); // Инициализация реле

//     // Выключаем все каналы при старте
//     pwrkey.digitalWrite(ALL_CHANNEL, HIGH);
// }

// void loop() {
//     float pH = sensor.getPH();
//     Serial.println(pH);
//     // Проверяем наличие данных в Serial1 для управления реле
//     if (Serial.available() > 0) {
//         char command = Serial.read(); // Читаем команду из UART
//         controlRelay(command); // Управляем реле в зависимости от команды
//     }
//     delay(3000);
//     // Можно добавить дополнительный код для работы с pH-датчиком, если это необходимо
// }

// // Функция для управления реле
// void controlRelay(char command) {
//     switch (command) {
//         case '1': // Включаем первый канал
//             pwrkey.digitalWrite(1, HIGH);
//             break;
//         case '2': // Выключаем первый канал
//             pwrkey.digitalWrite(1, LOW);
//             break;
//         case '3': // Включаем второй канал
//             pwrkey.digitalWrite(2, HIGH);
//             break;
//         case '4': // Выключаем второй канал
//             pwrkey.digitalWrite(2, LOW);
//             break;
//         case '5': // Включаем третий канал
//             pwrkey.digitalWrite(3, HIGH);
//             break;
//         case '6': // Выключаем третий канал
//             pwrkey.digitalWrite(3, LOW);
//             break;
//         case '7': // Включаем четвертый канал
//             pwrkey.digitalWrite(4, HIGH);
//             break;
//         case '8': // Выключаем четвертый канал
//             pwrkey.digitalWrite(4, LOW);
//             break;
//         case '0': // Выключаем все каналы
//             pwrkey.digitalWrite(ALL_CHANNEL, LOW);
//             break;
//     }
// }
#include <SoftwareWire.h>
#include "Adafruit_TCS34725_Soft.h" // Модифицированная библиотека

// Определяем пины для каждого датчика
#define SDA1 2
#define SCL1 3
#define SDA2 4
#define SCL2 5
#define SDA3 6
#define SCL3 7
#define SDA4 8
#define SCL4 9

// Создаем объекты SoftwareWire для каждого датчика
SoftwareWire Wire1(SDA1, SCL1);
SoftwareWire Wire2(SDA2, SCL2);
SoftwareWire Wire3(SDA3, SCL3);
SoftwareWire Wire4(SDA4, SCL4);

// Создаем объекты датчиков для каждого SoftwareWire
Adafruit_TCS34725 tcs1(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X, &Wire1);
Adafruit_TCS34725 tcs2(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X, &Wire2);
Adafruit_TCS34725 tcs3(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X, &Wire3);
Adafruit_TCS34725 tcs4(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X, &Wire4);

void setup() {
  Serial.begin(9600);

  // Инициализируем SoftwareWire и датчики
  Wire1.begin();
  if (tcs1.begin()) {
    Serial.println("Датчик 1 найден.");
  } else {
    Serial.println("Датчик 1 не найден.");
  }

  Wire2.begin();
  if (tcs2.begin()) {
    Serial.println("Датчик 2 найден.");
  } else {
    Serial.println("Датчик 2 не найден.");
  }

  Wire3.begin();
  if (tcs3.begin()) {
    Serial.println("Датчик 3 найден.");
  } else {
    Serial.println("Датчик 3 не найден.");
  }

  Wire4.begin();
  if (tcs4.begin()) {
    Serial.println("Датчик 4 найден.");
  } else {
    Serial.println("Датчик 4 не найден.");
  }
}

void loop() {
  uint16_t r, g, b, c;

  // Чтение с датчика 1
  tcs1.getRawData(&r, &g, &b, &c);
  Serial.print("Датчик 1 - R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print(" C: "); Serial.println(c);

  // Чтение с датчика 2
  tcs2.getRawData(&r, &g, &b, &c);
  Serial.print("Датчик 2 - R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print(" C: "); Serial.println(c);

  // Чтение с датчика 3
  tcs3.getRawData(&r, &g, &b, &c);
  Serial.print("Датчик 3 - R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print(" C: "); Serial.println(c);

  // Чтение с датчика 4
  tcs4.getRawData(&r, &g, &b, &c);
  Serial.print("Датчик 4 - R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print(" C: "); Serial.println(c);

  delay(1000);
}
