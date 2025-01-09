/*
  QuizButton

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  created 2025
  by Takae Hirayama
*/


#include <QList.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_ADDRESS 0x3C

// constants won't change. They're used here to set pin numbers:
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;      
const int OLED_TEXT_SIZE = 9;
const int NUM_OF_ANSWERERS = 5; // 回答者数
const int RESET_BUTTON_PIN = 5; // リセットボタンは5番ピン
const int CORRECT_BUTTON_PIN = RESET_BUTTON_PIN + 1; // 正解ボタンはリセットボタンの隣
const int WRONG_BUTTON_PIN = CORRECT_BUTTON_PIN + 1; // // 不正解ボタンは正解ボタンの隣
const int BUTTON_ON = HIGH; // ボタンを押した時に電圧がHIGHになる
const int BUTTON_OFF = LOW; // ボタンを離した時に電圧がLOWになる
const int EVENT_INTERVAL = 500; // イベントの間隔（ミリ秒）
int ANSWERER_BUTTON_PINS[NUM_OF_ANSWERERS]; // 回答ボタンのピン番号を格納する配列
int EFFECT_BUTTON_PINS[3] = {RESET_BUTTON_PIN, CORRECT_BUTTON_PIN, WRONG_BUTTON_PIN};  // 効果ボタンのピン番号を格納する配列
QList<int> answererQueue;
QList<int> toneQueue;
QList<int> durationQueue;

// variables will change:
int buttonState;  // variable for reading the pushbutton status
bool effectButtonPushedFlag;
int answererNumber;
int effectButtonNumber;
unsigned long currentMillis = 0;
unsigned long latestEventMillis = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  for (int i=0; i < NUM_OF_ANSWERERS; i++) {
    ANSWERER_BUTTON_PINS[i] = i;
    pinMode(i, INPUT);
  }
  for (int i=0; i < sizeof(EFFECT_BUTTON_PINS)/sizeof(int); i++) {
    pinMode(EFFECT_BUTTON_PINS[i], INPUT);
  }
  // OLED初期化
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 disconnection"));
    for(;;); 
  }
  clearDisplay();
}

void loop() {
  // 変数初期化
  buttonState = 0;
  effectButtonPushedFlag = false;
  currentMillis = millis();
  // 回答ボタンの入力を受付
  answererNumber = -1;
  detectAnswerButton();
  // 効果ボタンの入力を受付
  effectButtonNumber = -1;
  detectEffectButton();
  // 回答者キューに番号がある場合は番号を表示
  if (answererQueue.length() > 0) {
    displayNumber(answererQueue.front());
  } else {
    clearDisplay();
  }
}

void detectAnswerButton() {
  for (int i=0; i < NUM_OF_ANSWERERS; i++) {
    buttonState = digitalRead(ANSWERER_BUTTON_PINS[i]);
    if (buttonState == BUTTON_ON) {
      answererNumber = i + 1;
      // 回答者番号がまだキューになければキューに追加
      if (answererQueue.indexOf(answererNumber) < 0) {
        answererQueue.push_back(answererNumber);
        break;
      }
    }
  }
}

void detectEffectButton() {
  for (int i=0; i < sizeof(EFFECT_BUTTON_PINS)/sizeof(int); i++) {
    buttonState = digitalRead(EFFECT_BUTTON_PINS[i]);
    if (buttonState == BUTTON_ON) {
      // 最後のイベントから一定時間が経過していない場合、イベントを行わない
      if (currentMillis - latestEventMillis >= EVENT_INTERVAL) {
        latestEventMillis = currentMillis;
        effectButtonPushedFlag = true;
        effectButtonNumber = EFFECT_BUTTON_PINS[i];
        break;
      }
    }
  }
  if (effectButtonPushedFlag == true) {
    switch(effectButtonNumber) {
      case RESET_BUTTON_PIN:
        answererQueue.clear();
        break;
      case CORRECT_BUTTON_PIN:
        answererQueue.clear();
        break;
      case WRONG_BUTTON_PIN:
        answererQueue.pop_front();
        break;
    }
    effectButtonPushedFlag = false;
  }
}

void displayNumber(int number) {
  display.clearDisplay(); //初期化
  display.setTextColor(WHITE);
  display.setTextSize(OLED_TEXT_SIZE);
  display.setCursor(50, 0);
  display.println(number);
  display.display();
}

void clearDisplay() {
  display.clearDisplay(); //初期化
  display.display();
}