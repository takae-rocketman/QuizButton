/*
  QuizButton

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
#define SCREEN_ADDRESS 0x3C

// constants won't change. They're used here to set pin numbers:
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;
const int OLED_TEXT_SIXE = 9;
const int NUM_OF_ANSWERERS = 5; // 回答者数
const int RESET_BUTTON_PIN = 5; // リセットボタンは5番ピン
const int CORRECT_BUTTON_PIN = RESET_BUTTON_PIN + 1; // 正解ボタンはリセットボタンの隣
const int WRONG_BUTTON_PIN = CORRECT_BUTTON_PIN + 1; // // 不正解ボタンは正解ボタンの隣
const int BUTTON_ON = HIGH; // ボタンを押した時に電圧がHIGHになる
const int BUTTON_OFF = LOW; // ボタンを離した時に電圧がLOWになる
const int ANSWER_ACCEPTING_MODE = 0; // 回答ボタン操作受付中
const int DISPLAY_MODE = 1;   // 回答者の番号を表示中
int ANSWERER_BUTTON_PINS[NUM_OF_ANSWERERS]; // 回答ボタンのピン番号を格納する配列
int EFFECT_BUTTON_PINS[3] = {RESET_BUTTON_PIN, CORRECT_BUTTON_PIN, WRONG_BUTTON_PIN};  // 効果ボタンのピン番号を格納する配列

// variables will change:
int mode;
int buttonState;  // variable for reading the pushbutton status
bool buttonPushedFlag;

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
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);
  mode = ANSWER_ACCEPTING_MODE;
  // OLED初期化
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 disconnection"));
    for(;;); 
  }
  display.clearDisplay(); //初期化
  display.display();
}

void loop() {
  // 変数初期化
  buttonState = 0;
  buttonPushedFlag = false;
  if (mode == ANSWER_ACCEPTING_MODE) {
    int answererNumber;
    for (int i=0; i < NUM_OF_ANSWERERS; i++) {
      buttonState = digitalRead(ANSWERER_BUTTON_PINS[i]);
      if (buttonState == BUTTON_ON) {
        buttonPushedFlag = true;
        answererNumber = i;
        break;
      }
    }
    if (buttonPushedFlag == true) {
      display.setTextColor(WHITE);
      display.setTextSize(OLED_TEXT_SIXE);
      display.setCursor(50, 0);
      display.println(answererNumber + 1);
      display.display();
      mode = DISPLAY_MODE;
      buttonPushedFlag = false;
    }
    else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  } else if (mode == DISPLAY_MODE) {
    int pushedButtonNumber;
    for (int i=0; i < sizeof(EFFECT_BUTTON_PINS)/sizeof(int); i++) {
      buttonState = digitalRead(EFFECT_BUTTON_PINS[i]);
      if (buttonState == BUTTON_ON) {
        buttonPushedFlag = true;
        pushedButtonNumber = EFFECT_BUTTON_PINS[i];
        break;
      }
    }
    if (buttonPushedFlag == true) {
      switch(pushedButtonNumber) {
        case RESET_BUTTON_PIN:
          display.clearDisplay();
          display.display();
          break;
      }
      mode = ANSWER_ACCEPTING_MODE;
      buttonPushedFlag = false;
    }
  }
}