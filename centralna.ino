


#include "eprom.h"
#include <LiquidCrystal.h>
#include "delayedFunction.h"
#include "keys.h"
#include "display.h"
#include <limits.h>

#include <OneWire.h>
#include <DallasTemperature.h>


float tempLow = 20.0f;
float tempHigh = 40.0f;
float tempCurr = 32.0f;


OneWire oneWire(2);
DallasTemperature sensors(&oneWire);

void checkTemperature()
{
  
  tempCurr = sensors.getTempCByIndex(0);
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  addDelayFunction(checkTemperature, 750);
}

int relayDelay = 60;
bool timerMode = false;
bool useLowTemperature = false;
int startTime = 3600;
int timer = 0;
void decreaseTimer()
{
  if(timer > 0)
    timer--;
  useLowTemperature = !(timer == 0);
  addDelayFunction(decreaseTimer,1000);
}

void initTimer()
{
  addDelayFunction(decreaseTimer,748);
}

bool processKey(int code,int duration)
{
  if(code == VK_RETURN && timerMode)
  {
    timer = startTime;
    return true;
  }
  return false;
}

void initTemperature()
{
  pinMode(2,OUTPUT);
  sensors.begin();
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  addDelayFunction(checkTemperature, 123);
}

bool tempMode = 0;

int delayCount = 0;
void relayOnOff()
{
  if(delayCount>=relayDelay)
  {
    delayCount=0;
 
    if(timerMode)
    {
      if(useLowTemperature)
      {
        if(tempLow < tempCurr)
          digitalWrite(12,HIGH);
        else
          digitalWrite(12,LOW);
      }
      else
      {
        if(tempHigh < tempCurr)
          digitalWrite(12,HIGH);
        else
          digitalWrite(12,LOW);
      }
    }
    else
    {
      if(tempCurr<tempLow)
      {
        tempMode=0;
        digitalWrite(12,LOW);
      }
      if(tempCurr>tempHigh)
      {
        tempMode=1;
        digitalWrite(12,HIGH);
      }
      if(tempMode)
      {
        if(tempCurr<tempHigh)
        {
          digitalWrite(12,LOW);
        }
        else
        {
          digitalWrite(12,HIGH);
        }
      }
      else
      {
        if(tempCurr<tempLow)
        {
          digitalWrite(12,LOW);
        }
        else
        {
          digitalWrite(12,HIGH);
        }
      }
    }
  }
  delayCount++;
  addDelayFunction(relayOnOff, 1000);
}

void initRelay()
{
  pinMode(12, OUTPUT);
   addDelayFunction(relayOnOff, 2387);
}

LiquidCrystal lcd(5, 4, 9, 8, 7, 6);

void displayInitLCD()
{
  delay(100);
  
  lcd.begin(2,16);
  delay(100);
  byte C[8] = {
    B01010,
    B00100,
    B01110,
    B10001,
    B10000,
    B10001,
    B01110,
    B00000
  };
  lcd.createChar(1, C);
  delay(100);
  byte S[8] = {
    B01010,
    B00100,
    B01110,
    B10000,
    B01110,
    B00001,
    B11110,
    B00000
  };
  lcd.createChar(2, S);

  byte Z[8] = {
    B01010,
    B00100,
    B11111,
    B00010,
    B00100,
    B01000,
    B11111,
    B00000
  };
  lcd.createChar(3, Z);
  delay(100);
  byte l[8] = {
    B00010,
    B00110,
    B01110,
    B11110,
    B01110,
    B00110,
    B00010,
    B00000
  };
  lcd.createChar(4, l);
  delay(100);
  byte r[8] = {
    B01000,
    B01100,
    B01110,
    B01111,
    B01110,
    B01100,
    B01000,
    B00000
  };
  lcd.createChar(5, r);
  delay(100);
  byte c[8] = {
    B01000,
    B10100,
    B01000,
    B00110,
    B01001,
    B01000,
    B01001,
    B00110
  };
  lcd.createChar(6, c);
  delay(100);

  byte k[8] = {
    B00100,
    B01110,
    B11111,
    B00100,
    B00100,
    B11111,
    B11111,
    B00000
  };
  lcd.createChar(7, k);
  delay(100);

  byte p[8] = {
    B01110,
    B10001,
    B10101,
    B11111,
    B10101,
    B10001,
    B01110,
    B00000
  };
  lcd.createChar(0, p);
  delay(100);


  //return lcd;
}

Display *disp;

void addMenuItem(void* dsp)
{
  Display* d = (Display*)dsp;
  d->AddMenuItem(new FloatSelectMenuItem("Low Temperature ", &tempLow, 0.0, 99.9, 0.1, NULL, 1, 0, 0));
  d->AddMenuItem(new FloatSelectMenuItem("High Temperature", &tempHigh, 0.0, 99.9, 0.1, NULL, 1, 0, 0));
  d->AddMenuItem(new BoolSelectMenuItem("Timer/Auto Mode  ", &timerMode, 'T', 'A', NULL, true, 0, 0));
  d->AddMenuItem(new TimeSelectMenuItem("Timer Time      ", &startTime, 0, INT_MAX, NULL, 1, 0, 0));
  d->AddMenuItem(new TimeSelectMenuItem("Switch Delay    ", &relayDelay, 1, INT_MAX, NULL, 1, 0,0));
  d->AddMenuItem(new IntSelectMenuItem("Light Intensity ", &(d->Light), 0, 255, &displaySetLight, 1, 0, 0));
  d->AddMenuItem(new IntSelectMenuItem("Light Time      ", &(d->InactiveTime), 0, 90, NULL, 1, 0, 0));
  
  d->AddMenuItem(new DefaultSettingsMenuItem());
}

void epromInit()
{
  Eprom::Add(&tempLow);
  Eprom::Add(&tempHigh);
  Eprom::Add(&startTime);
  Eprom::Add(&timerMode);
  Eprom::Add(&(disp->Light));
  Eprom::Add(&(disp->InactiveTime));
  Eprom::Add(&relayDelay);
  Eprom::Load();
}

void displayKeys()
{
  int code;
  int duration;
  readKeys(&code, &duration);
  disp->ProcessKey(code, duration);

  addDelayFunction(displayKeys, 100);
}
void displayUpdate()
{
  disp->Update();
  addDelayFunction(displayUpdate, 1000);
}
void displaySetLight()
{
  disp->SetLight();
}

void displayInit()
{
  Display*d = new Display(&lcd, (byte)3, (byte)16, (byte)2, "  , \6        , \6", "  , \6           ", addMenuItem);
  d->AddDisplayVar(new FloatSelectMenuItem("Temperatura     ", &tempLow, 0.0, 99.9, 0.1, NULL, 0, 0, 0));
  d->AddDisplayVar(new FloatSelectMenuItem("Vla\3nost        ", &tempHigh, 0.0, 99.9, 0.1, NULL, 0, 0, 1));
  d->AddDisplayVar(new FloatSelectMenuItem("Vla\3nost        ", &tempCurr, 0.0, 99.9, 0.1, NULL, 0, 11, 0));
  d->AddDisplayVar(new TimeSelectMenuItem("Timer time      ", &timer, 0, INT_MAX, NULL, 1, 8, 1));
  d->AddDisplayVar(new BoolSelectMenuItem("Timer/Auto mode  ", &timerMode, 'T', 'A', NULL, true, 6, 1));
  d->processHomeKeys = &processKey;
  d->Show();

  disp = d;
  d->InitTotalItems();
  
  addDelayFunction(displayKeys, 123);
  addDelayFunction(displayUpdate, 534);
 


}


void setup() {
  displayInitLCD();
  // put your setup code here, to run once;
  delay(100);
  lcd.clear();
  delayedFunctInit();
  lcd.print("MeMarko         ");
  lcd.setCursor(0,1);
  lcd.print("        (c) 2016");
  delay(3000);
  displayInit();
  epromInit();
  displaySetLight();
  initTemperature();
  initTimer();
  initRelay();
  
  delay(3000);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  execDelayFunction();
}

  


