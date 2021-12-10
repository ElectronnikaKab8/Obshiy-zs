#include <AmperkaKB.h> //Матричная клавиатура
#include <LiquidCrystal.h> //Экран
//#include <NewPing.h> //УЗ модуль
#include <TroykaRTC.h> //Часы
#include <Wire.h> //Часы
#include <Servo.h> //Микросервопривод
#include <TroykaDHT.h> //Датчик температуры и влажности
//#include <Adafruit_NeoPixel.h> //RGB матрица
#include <TroykaLight.h> //Датчик освещенности

#define POMP_PIN 6 //Пин поделючения помпы

#define swimhour 3 //Время полива ч.
#define swimminutes 4 //Время полива мин.

//#define WIFI_SERIAL    Serial1 //Путь вывода Wifi модуля

#define LEN_TIME 12 //Массив времени
#define LEN_DATE 12 //Массив дат
#define LEN_DOW 12 //День недели

#define TRIGGER_PIN  8 //Пин подключения Trig УЗ
#define ECHO_PIN     9 //Пин подключения Echo УЗ
#define MAX_DISTANCE 400 //Максимальное расстояние УЗ

#define Xcord  A0 //X джойстика
#define Ycord  A1 //Высота джойстика
#define Zcord  A2 //Z джойстика

#define MATRIX_PIN 13 //Подключение матрицы
#define LED_COUNT 16 //Количество светодиодов

#define MOTOR_PIN 8 //Мотор (мб и не нужен)

#define r 60

TroykaLight sensorLight(A4); //Датчик освещенности

DHT dht(7, DHT11); //Датчик температуры и влажности

RTC clock; //Время

char time[LEN_TIME]; //Время
char date[LEN_DATE]; //Дата
char weekDay[LEN_DOW]; //День недели

Servo myservo; //Мотор myservo

//Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_COUNT, MATRIX_PIN, NEO_GRB + NEO_KHZ800); //Значения для матрицы
//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //Библиотека под УЗ
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //Подключение LSD
AmperkaKB KB(30, 29, 28, 27, 26, 25, 24, 23); //Подключение матричной клавиатуры

int svet; //Переменная минемального уровня света 

int tempchisla; //Переменная минимальной температуры

int vlaga; //Переменная минимальной влажности

boolean flag = true; //Флаг от бесконечных повторов

boolean thunder = 0; //Нужен ли вывод оповещения о свете?

boolean t = 0; //Нужно ли вывести оповещение что понизить температуру?

boolean voda = 0; //Нужно ли вывести оповещение о поливе?

boolean f = true;

void setup() {
  pinMode(53, OUTPUT);
  pinMode(31, OUTPUT);
//  matrix.begin();
  lcd.begin(20, 4);
  myservo.attach(6);
  Serial.begin(9600);
  while (!Serial) {}
  Serial.print("Serial init OK\r\n");
//  WIFI_SERIAL.begin(115200);
  pinMode(14, INPUT);
  clock.begin();
  clock.set(__TIMESTAMP__);
  dht.begin();
  KB.begin(KB4x4);
  pinMode(POMP_PIN, OUTPUT);
  pinMode(r, OUTPUT);
  sensorLight.read();
  int a1 = sensorLight.getLightLux();
  delay(1000);
  int a2 = sensorLight.getLightLux();
  delay(1000);
  int a3 = sensorLight.getLightLux();
  delay(1000);
  svet = (a1 + a2 + a3) / 3;
}

void thunderUndLight() { // Функция света
    sensorLight.read();
    KB.read();
    Serial.println(sensorLight.getLightLux());
    if (sensorLight.getLightLux() > svet) {
          //for (uint16_t i = 0; i < matrix.numPixels(); i++) {
          //  matrix.setPixelColor(i, (255, 255, 0));
          //  matrix.show();
          //}
          digitalWrite(53, HIGH);
          delay(1000);
          digitalWrite(53, LOW);
          thunder = true;
    }
    else if (KB.justPressed()) {
        Serial.println(KB.getChar);
        Serial.println(f);
        if (KB.getChar == '1' && f) {
          //for (uint16_t i = 0; i < matrix.numPixels(); i++) {
          //  matrix.setPixelColor(i, (255, 255, 0));
          //  matrix.show();
          //}
          digitalWrite(53, HIGH);
          thunder = true;
          f = false;
        }  
        else if (KB.getChar == '1' && !f) {
          digitalWrite(53, LOW);
          thunder = true;
          f = true;
        }
    }
    else {
        //for (uint16_t i = 0; i < matrix.numPixels(); i++) {
        //   matrix.setPixelColor(i, (0, 0, 0));
        //    matrix.show();
        //}
        digitalWrite(53, LOW);
        f = false;
    }
}

void tempur() { //Функция температуры
  if (dht.getTemperatureC() > tempchisla) {
      digitalWrite(r, 1);
      t = true;
  }
  else if (KB.justPressed()) {
      if (KB.getChar == '2') {
          digitalWrite(r, 1);    
          t = true;
      }
  }
  else {
      digitalWrite(r, 0);
  }
}

void poliv() { //Функция полива
  flag == true;
  if ((clock.getHour() == swimhour && clock.getMinute() == swimminutes && clock.getSecond() == 0) || dht.getHumidity() < vlaga) {
    digitalWrite(POMP_PIN, HIGH);
    delay(10000);
    voda = true;
  }
  if (KB.justPressed()) {
      if (KB.getChar == '3') {
          digitalWrite(POMP_PIN, HIGH);
          delay(10000);
          voda = true;
      }  
  }
  if (flag) {
    flag == false;  
    digitalWrite(POMP_PIN, LOW);
  }
}

void vivod() { //Вывод данных и управление теплицей
    lcd.setCursor(0, 0);
    lcd.print(clock.getHour());
    lcd.print(":");
    lcd.print(clock.getMinute());
    lcd.setCursor(0, 1);
    lcd.print("Температура = ");
    lcd.print(dht.getTemperatureC());
    lcd.setCursor(0, 2);
    lcd.print("Влажность = ");
    lcd.print(dht.getHumidity());
    lcd.setCursor(0, 3);
    lcd.print("Освещенность = ");
    lcd.print(sensorLight.getLightLux());
    if (thunder) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Включен свет!");
        delay(5000);
        lcd.clear();
        thunder = false;  
    }
    if (t) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Вентелятор?");
        delay(5000);
        lcd.clear(); 
        t = false;
    }
    if (voda) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Полито!");
        delay(5000);
        lcd.clear();
        voda = false; 
    }
}

/* Мне пока не понятно зачем в теплице джойстик и ультразвуковой 
дальномер*/ 

void loop() {
  thunderUndLight(); //Вызов функции света 
  tempur(); //Вызов функции температуры
  poliv(); //Вызов функции полива растений
  vivod(); //Вывод данных
}
