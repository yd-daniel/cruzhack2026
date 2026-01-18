#include <Arduino.h>
#include <IntervalTimer.h> //Found this for Teensy 3.0 online
// ^ we could always just use a while loop

//u8g2library for oled
#include <Wire.h>
#include <U8g2lib.h>

#include <stdio.h>
#include <math.h>

//include our favorite headers
#include "RollingAverage.h"
#include "Oled.h"

//Menu
int menuMode; //1 = Bar Graph, 2 = Summary, 3 = Sleep, 10< = warning

//Constants
const int HZ = 70; //Refresh rate of the oled panel
const int OLED_REFRESH = 7; //May be different from hz
const int LOUD = 80; //Temporary loud dB threshold

//Volume
int dB; //Variable for how loud the noise is
RollingAverage decibelAvg; //Needs to be declared
RollingAverage decibelAvg1s;

//Flags
volatile bool timer = false; //Main cooldown timer
int time;      //How much time is left on the timer
volatile bool buttonIsPressed = false; //Main multi-purpose button
bool potentiometerOutOfBounds = false;
bool warningOnDisplay = false;
bool oledUpdateAvailable = true;

//Make a timer object?
IntervalTimer mainTimer;

//Until
int untilMainDisplayUpdates;
int untilDecibelDisplayUpdates;
int untilRollingAvgUpdates;

//Pins
const int POT_PIN = 20;
const int MIC_PIN = 15;
const int BUTTON_PIN = 1;
const int MOTOR_PIN = 14;

const int MAIN_SCL = 19;
const int MAIN_SDA = 18;
const int DECIBEL_SCL = 16;
const int DECIBEL_SDA = 17;

const int MAIN_OLED_PERIOD_MS = HZ / OLED_REFRESH;
const int DECIBEL_OLED_PERIOD_MS = HZ / OLED_REFRESH;
const int ROLLING_AVG_PERIOD_MS = HZ / 7;

const int HISTORY_SIZE = 28;
const int ONE_SEC_SIZE = 7;
const float DT_SECONDS = 1.0f / 7.0f;

static float decibelBacking[HISTORY_SIZE] = {0};
static float oneSecBacking[ONE_SEC_SIZE] = {0};
static float decibelHistory[HISTORY_SIZE] = {0};

static char mainXbm[1024];
static char decibelXbm[512];

Oled oledState;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2Main(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_2ND_HW_I2C u8g2Db(U8G2_R0, U8X8_PIN_NONE);

bool sleepActive = false;
unsigned long sleepTicks = 0;
unsigned long loudEvents = 0;
int loudStreak = 0;
float warningDose = 0.0f;

//Interrupts
void startTimer(){
timer = true;
}

void buttonPress(){
buttonIsPressed = true;
}

//Potentiometer voltage standard integer placeholders
int readMenuFromPotentiometer(){
  int v = analogRead(POT_PIN);
  if (v < 341) return 1;
  if (v < 682) return 2;
  return 3;
}

//Placeholder integer values are for a normal microphone input
int readDbFromMic(){
  int v = analogRead(MIC_PIN);
  int db = map(v, 0, 1023, 0, 100);
  if (db < 0) db = 0;
  if (db > 120) db = 120;
  return db;
}

void pushHistory(float sample){
  for(int i = 0; i < HISTORY_SIZE - 1; i++){
    decibelHistory[i] = decibelHistory[i + 1];
  }
  decibelHistory[HISTORY_SIZE - 1] = sample;
}

float allowableSecondsFromDb(float db){
  float x = (db - 85.0f) / 3.0f;
  float base = 28800.0f;
  float denom = powf(2.0f, x);
  if (denom < 1.0e-6f) denom = 1.0e-6f;
  float t = base / denom;
  if (t < 1.0f) t = 1.0f;
  return t;
}

void renderMain(int dbNow, float avg4sDb){
  u8g2Main.clearBuffer();
  u8g2Main.drawXBM(0, 0, 128, 64, (const uint8_t*)mainXbm);

  if(menuMode == 2){
    char s[24];
    snprintf(s, sizeof(s), "%lu", loudEvents);
    u8g2Main.setFont(u8g2_font_6x10_tf);
    u8g2Main.drawStr(10, 40, s);
  }

  if(menuMode == 3){
    unsigned long sec = sleepTicks / (unsigned long)HZ;
    unsigned long m = sec / 60UL;
    unsigned long s2 = sec % 60UL;

    char tbuf[24];
    snprintf(tbuf, sizeof(tbuf), "%lu:%02lu", m, s2);
    u8g2Main.setFont(u8g2_font_6x10_tf);
    u8g2Main.drawStr(10, 40, tbuf);
  }

  if(warningOnDisplay){
    float avg = avg4sDb;
    float allow = allowableSecondsFromDb(avg);
    float rem = allow * (1.0f - warningDose);
    if (rem < 0.0f) rem = 0.0f;

    unsigned long sec = (unsigned long)(rem + 0.5f);
    unsigned long m = sec / 60UL;
    unsigned long s2 = sec % 60UL;

    char tbuf[24];
    snprintf(tbuf, sizeof(tbuf), "%lu:%02lu", m, s2);
    u8g2Main.setFont(u8g2_font_6x10_tf);
    u8g2Main.drawStr(50, 56, tbuf);
  } else {
    char s1[12];
    char s2[12];
    snprintf(s1, sizeof(s1), "%ddB", dbNow);
    snprintf(s2, sizeof(s2), "avg %d", (int)(avg4sDb + 0.5f));

    u8g2Main.setFont(u8g2_font_6x10_tf);
    u8g2Main.drawStr(92, 12, s1);
    u8g2Main.drawStr(92, 24, s2);
  }

  u8g2Main.sendBuffer();
}

void renderDb(float avg1sDb){
  u8g2Db.clearBuffer();
  u8g2Db.drawXBM(0, 0, 128, 32, (const uint8_t*)decibelXbm);

  char s1[18];
  snprintf(s1, sizeof(s1), "%d", (int)(avg1sDb + 0.5f));

  u8g2Db.setFont(u8g2_font_6x10_tf);
  u8g2Db.drawStr(92, 14, s1);

  u8g2Db.sendBuffer();
}

void setup() {

    analogReadResolution(10);


  //Pins
  Wire.begin();
  Wire1.begin();

  pinMode(POT_PIN, INPUT);
  pinMode(MIC_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPress, FALLING);

  menuMode = 1;
  dB = 0;
  time = 0;

  untilMainDisplayUpdates = 0;
  untilDecibelDisplayUpdates = 0;
  untilRollingAvgUpdates = 0;

  RollingAverage_init(&decibelAvg, decibelBacking, HISTORY_SIZE);
  RollingAverage_init(&decibelAvg1s, oneSecBacking, ONE_SEC_SIZE);

  Oled_init(&oledState);

  u8g2Main.begin();
  u8g2Db.begin();

  mainTimer.begin(startTimer, 1000000 / HZ);
}

//loop
void loop() {
  if(timer){
    timer = false;
    if(untilMainDisplayUpdates > 0) untilMainDisplayUpdates--;
    if(untilDecibelDisplayUpdates > 0) untilDecibelDisplayUpdates--;
    if(untilRollingAvgUpdates > 0) untilRollingAvgUpdates--;
    if(time > 0) time--;
    if(sleepActive) sleepTicks++;
  }

  if(buttonIsPressed){
    buttonIsPressed = false;

    if(warningOnDisplay){
      warningOnDisplay = false;
      oledUpdateAvailable = true;
      menuMode = menuMode -10; //This is to make it go back to the previous screen
      warningDose = 0.0f;
      loudStreak = 0;
    } else {
      if(menuMode == 3){
        sleepActive = !sleepActive;
        sleepTicks = 0;
        oledUpdateAvailable = true;
      }
    }
  }

int newMenuMode = readMenuFromPotentiometer();
  potentiometerOutOfBounds = (newMenuMode != menuMode);

if (potentiometerOutOfBounds && !warningOnDisplay) {
  menuMode = newMenuMode;
  oledUpdateAvailable = true;
  if(menuMode != 3){
    sleepActive = false;
    sleepTicks = 0;
  }
}

if(untilRollingAvgUpdates <= 0){
  untilRollingAvgUpdates = ROLLING_AVG_PERIOD_MS;

  dB = readDbFromMic();

  RollingAverage_addValue(&decibelAvg, (float)dB);
  RollingAverage_addValue(&decibelAvg1s, (float)dB);
  pushHistory((float)dB);

  digitalWrite(MOTOR_PIN, (dB >= LOUD) ? HIGH : LOW);

  if(dB >= LOUD){
    loudStreak++;
    if(loudStreak > HISTORY_SIZE) loudStreak = HISTORY_SIZE;
  } else {
    loudStreak = 0;
  }

  if(!warningOnDisplay && loudStreak >= HISTORY_SIZE){
    warningOnDisplay = true;
    oledUpdateAvailable = true;
    menuMode = menuMode + 10;
    loudEvents++;
    warningDose = 0.0f;
  }
}

float avg4sDb = RollingAverage_getAverage(&decibelAvg);
float avg1sDb = RollingAverage_getAverage(&decibelAvg1s);

if(warningOnDisplay){
  float allow = allowableSecondsFromDb(avg4sDb);
  warningDose += (DT_SECONDS / allow);
  if(warningDose < 0.0f) warningDose = 0.0f;
  if(warningDose > 1.0f) warningDose = 1.0f;
}

if(untilMainDisplayUpdates <= 0){
  untilMainDisplayUpdates = MAIN_OLED_PERIOD_MS;

  if(oledUpdateAvailable || warningOnDisplay){
    if(warningOnDisplay){
      Oled_getWarningXBM(mainXbm, warningDose);
      renderMain(dB, avg4sDb);
    } else {
      if(menuMode == 1){
        Oled_getBarChartXBM(mainXbm, decibelHistory);
        renderMain(dB, avg4sDb);
      }
      if(menuMode == 2){
        Oled_getSummaryXBM(mainXbm);
        renderMain(dB, avg4sDb);
      }
      if(menuMode == 3){
        Oled_getSleepXBM(mainXbm);
        renderMain(dB, avg4sDb);
      }
    }

    oledUpdateAvailable = false;
  }
}

if(untilDecibelDisplayUpdates <= 0){
  untilDecibelDisplayUpdates = DECIBEL_OLED_PERIOD_MS;
  Oled_getDecibelXBM(decibelXbm);
  renderDb(avg1sDb);
}

}
