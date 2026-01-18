#include <Arduino.h>
#include <IntervalTimer.h> //Found this for Teensy 3.0 online
// ^ we could always just use a while loop

//include our favorite headers
#include "RollingAverage.h"
#include "Oled.h"

// put function declarations here:
int myFunction(int, int);

//Menu
int menuMode; //1 = Bar Graph, 2 = Summary, 3 = Sleep, 10< = warning

//Constants
const int hz = 20; //Refresh rate of the oled panel
const int oledRefresh = 5; //May be different from hz
const int loud = 80; //Temporary loud dB threshold

//Volume
#define global
global int dB; //Variable for how loud the noise is
avg decibelAvg; //Needs to be declared

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


//Interrupts
void startTimer(){
timer = true;
}

void buttonPress(){
buttonIsPressed = true;
}

//loop
void loop() {
  if(timer){
    timer = false;
    if(untilMainDisplayUpdates > 0) untilMainDisplayUpdates--;
    if(untilDecibelDisplayUpdates > 0) untilDecibelDisplayUpdates--;
    if(untilRollingAvgUpdates > 0) untilRollingAvgUpdates--;
    if(time > 0) time--;
  }

  if(buttonIsPressed){
    buttonIsPressed = false;
    if(warningOnDisplay){
    warningOnDisplay = false;
    oledUpdateAvailable = true;
    menuMode = menuMode -10; //This is to make it go back to the previous screen
    }  
  }

int newMenuMode = readMenuFromPotentiometer();
  potentiometerOutOfBounds = (newMenuMode != menuMode);

if (potentiometerOutOfBounds && !warningOnDisplay) {
  menuMode = newMenuMode;
  oledUpdateAvailable = true;
}

if(untilMainDisplayUpdates <= 0){
  untilMainDisplayUpdates = mainOledPeriodMs;

  if(oledUpdateAvailable || warningOnDisplay){
    if(warningOnDisplay){
      oledShowWarning("Warning: Loud Environment);
        }
}
}
}
// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}
