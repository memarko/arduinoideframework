
#include "display.h"
#include <Arduino.h>
#include "keys.h"
#include "eprom.h"

void displayInitLCD();
/*******************Main menu****************************************************************/
MainMenu::~MainMenu() {}
MainMenu::MainMenu(const char*template1, const char*template2,  void (*menuSelect)(void*))
{
  this->line1 = template1;
  this->line2 = template2;
  this->menuSelect = menuSelect;

}
void MainMenu::InitTotalItems()
{
  this->counter = 0;
  MenuSelect();
  this->totalItems = this->counter;
}
void MainMenu::Show(LiquidCrystal* lcd)
{
  //lcd->clear();
  if (currItem == NULL)
  {
    lcd->setCursor(0, 0);
    lcd->print(this->line1);
    lcd->setCursor(0, 1);
    lcd->print(this->line2);
  }
  else
  {

    currItem->ShowMenu(lcd);
  }
}


void MainMenu::MenuSelect()
{
  menuSelect(this);
}

void MainMenu::ProcessKey(int keycode, int duration,LiquidCrystal* lcd)
{
  if (currItem != NULL && currItem->IsLocked) {
    currItem->ProcessKey(keycode, duration, lcd);
    return;
  }
  switch (keycode)
  {
    case VK_DOWN:
      pos++;
      if (pos >= totalItems)
        pos = -1;
      counter = 0;
      if (currItem != NULL)
        delete currItem;
      currItem = NULL;

      MenuSelect();

      if (currItem == NULL)
      {
        Eprom::Store();
      }
      this->Show(lcd);

      break;
    case VK_UP:
      pos--;
      if (pos < -1)
        pos = totalItems - 1;
      counter = 0;
      if (currItem != NULL)
        delete currItem;
      currItem = NULL;

      MenuSelect();

      if (currItem == NULL)
      {
        Eprom::Store();
      }
      this->Show(lcd);

      break;
    case 0:
      break;
    default:
      if (currItem != NULL)
        currItem->ProcessKey(keycode, duration, lcd);
  }
}
void MainMenu::AddMenuItem(MenuItem* item)
{

  if (counter == pos)
  {
    currItem = item;
  }
  else
  {
    delete item;
  }
  this->counter++;
}
void MainMenu::AddDisplayVar(DisplayVar * item)
{
  DisplayVar** mi;
  for (mi = &displayVar; *mi != NULL; mi = &((*mi)->Next));
  *mi = item;
}

void MainMenu::Update(LiquidCrystal* lcd)
{
  if (currItem == NULL)
  {
    for (DisplayVar* dv = displayVar; dv != NULL; dv = dv->Next)
      dv->ShowVar(lcd);
  }
  else
  {
    currItem->ShowMenu(lcd);

  }
}

/*******************SubMenu****************************************************************/
SubMenu::~SubMenu()
{
  if (currItem != NULL)
        delete currItem;
    currItem = NULL;
}
SubMenu::SubMenu(const char*line1, const char*line2,  void (*menuSelect)(void*))
  : MainMenu(line1, line2, menuSelect)
{
  InitTotalItems();
  line1b = line1;
  line2b = line2;
  
}

void SubMenu::MenuSelect()
{
  MainMenu::MenuSelect();
}


void SubMenu::ShowMenu(LiquidCrystal* lcd)
{
  if (!IsLocked)
  {
    if (currItem != NULL)
        delete currItem;
    currItem = NULL;
  }
  MainMenu::Show(lcd);
}

void SubMenu::ProcessKey(int keycode, int duration, LiquidCrystal* lcd)
{
  if(!IsLocked)
  {
    switch(keycode)
    {
      case VK_RETURN:
      case VK_RIGHT:
      case VK_LEFT:
          IsLocked = 1;
          line1 = "<- Nazaj        ";
          line2 = "                ";
          counter = 0;
          pos=0;
          MenuSelect();
          ShowMenu(lcd);
    }
    return;
  }
  
  if(IsLocked)
  {
    if(currItem==NULL)
    {
      switch(keycode)
      {
        case VK_RETURN:
        case VK_RIGHT:
        case VK_LEFT:
            IsLocked = 0;
            line1 = line1b;
            line2 = line2b;
            ShowMenu(lcd);
            return;
      }
    }
    MainMenu::ProcessKey(keycode, duration,lcd);
  }
}

/*******************Display****************************************************************/
void Display::SetLight()
{
  if (lightPin >= 0)
    analogWrite(lightPin, Light);
  if(screenDimmed)
    displayInitLCD();
  lcd->display();
  screenDimmed = 0;
}
void Display::DimScreen()
{
  if (lightPin >= 0)
    analogWrite(lightPin, 0);
  lcd->noDisplay();
  screenDimmed = 1;
}

void Display::ToogleAlarm()
{
  alarm = !alarm;
  if (alarm)
  {
    if (lightPin >= 0)
      analogWrite(lightPin, Light);
  }
  else
  {
    if (lightPin >= 0)
      analogWrite(lightPin, 0);
  }
}
Display::~Display()
{
  
}
Display::Display(LiquidCrystal* lcd, byte lightPin, byte lcdCols, byte lcdRows, const char*template1, const char*template2, void (*menuSelect)(void*))
  :
  MainMenu(template1, template2, menuSelect)
{
  if (lightPin >= 0)
    pinMode(lightPin, OUTPUT);
  this->lightPin = lightPin;
  this->lcd = lcd;
  lcd->noCursor();
}

void Display::Update()
{
  if (currentInactiveTime == 60)
  {
    if (currItem != NULL)
    {
      Eprom::Store();
      currItem->IsLocked = 0;
      delete currItem;
    }
    currItem = NULL;
    Show();
  }
  if (currentInactiveTime >= InactiveTime && InactiveTime != 0)
  {

    DimScreen();
  }
  else
  {
    MainMenu::Update(lcd);
  }
  currentInactiveTime++;
}
void Display::ProcessKey(int keycode, int duration)
{
  if (keycode != 0)
  {
    SetLight();
    currentInactiveTime = 0;
  }
  //process home screen keys of process menu keys
  bool process = true;
  
  if (currItem == NULL || processHomeKeys == NULL)
      process = !processHomeKeys(keycode,duration);
  if(process)
    MainMenu::ProcessKey(keycode, duration, lcd);
}
void Display::Show()
{
  MainMenu::Show(lcd);
}

/*******************FloatSelectMenuItem****************************************************************/

float FloatSelectMenuItem::modRest(float a, float b)
{
  return floor(a / b) * b;
  //return (floor(floor(a / b*2.f+1.f))/2.f) * b;
}
float FloatSelectMenuItem::calculateStep(int count, float*value, int dir)
{
  float diff = (dir == 1) ? maxVal - minVal : *value;

  if (count <= 5 || diff / delta < 20)
    return delta;
  if (count <= 15 || diff / 5 / delta < 20)
  {
    (*value) = modRest(*value, 5.*delta);
    return 5.*delta;
  }
  else if (count <= 20  || diff / 10 / delta < 20)
  {
    (*value) = modRest(*value, 10.*delta);
    return 10.*delta;
  }
  else if (count <= 25 || diff / 50 / delta < 20)
  {
    (*value) = modRest(*value, 50.*delta);
    return 50.*delta;
  }
  else if (count <= 30 || diff / 100 / delta < 20)
  {
    (*value) = modRest(*value, 100.*delta);
    return 100.*delta;
  }
  else if (count <= 40 || diff / 500 / delta < 20)
  {
    (*value) = modRest(*value, 500.*delta);
    return 500.*delta;
  }
  else
  {
    (*value) -= modRest(*value, 1000.*delta);
    return 1000.*delta;
  }
}
void FloatSelectMenuItem::ShowValue(LiquidCrystal* lcd, bool pos)
{
  lcd->setCursor(col, row);
  int padding = 0;
  for (int mv = (int)maxVal; mv != 0; mv = mv / 10)
    padding++;
  int decimalPlaces = 0;
  int decimals;
  for (decimals = 1; floor(delta * decimals) <= 0; decimals = decimals * 10)
    decimalPlaces++;
  if (decimals > 1)
    decimalPlaces++;

  int intValue = (int) (*value * decimals);
  
  if (!pos)
  {
    lcd->setCursor(1,1);
    for(int i=1; i<(16 - padding - decimalPlaces)/2;i++)
      lcd->print(" ");
    lcd->setCursor((16 - padding - decimalPlaces) / 2 + padding + decimalPlaces, 1);
    for(int i=(16 - padding- decimalPlaces) / 2 + padding + decimalPlaces;i<15;i++)
      lcd->print(" ");
    lcd->setCursor((16 - padding- decimalPlaces) / 2, 1);
  }
  for (int mv = intValue/decimals; mv != 0; mv = mv / 10)
    padding--;
  if (floor(*value) == 0.)
    padding--;
  for (int i = 0; i < padding; i++)
    lcd->print(" ");

  lcd->print(intValue/decimals);
  if (decimals > 1)
    lcd->print(",");
  if (decimals > 1)
  {
    int valtoprint = intValue - (intValue/decimals)*decimals;
    for(decimals = decimals/10; decimals>valtoprint;decimals = decimals/10)
      lcd->print("0");
    if(valtoprint != 0)
      lcd->print(valtoprint);
  }
  if (isEditable && !pos)
  {
    lcd->setCursor(0, 1);
    if (*value > minVal)
      lcd->print("\4");
    else
      lcd->print(" ");
    lcd->setCursor(15, 1);
    if (*value < maxVal)
      lcd->print("\5");
    else
      lcd->print(" ");
  }
}

FloatSelectMenuItem::~FloatSelectMenuItem()
{
  
}
FloatSelectMenuItem::FloatSelectMenuItem(const char* label, float* value, float minVal, float maxVal, float delta, void (*callback)(), bool isEditable, byte col, byte row)
{
  this->label = label;
  this->minVal = minVal;
  this->maxVal = maxVal;
  this->value = value;
  this->callback = callback;
  this->isEditable = isEditable;
  this->col = col;
  this->row = row;
  this->delta = delta;
}
void FloatSelectMenuItem::ShowMenu(LiquidCrystal* lcd)
{
  lcd->setCursor(0, 0);
  lcd->print(label);
  ShowValue(lcd, 0);
}

void FloatSelectMenuItem::ShowVar(LiquidCrystal* lcd)
{
  ShowValue(lcd, 1);
}

void FloatSelectMenuItem::ProcessKey(int keycode, int duration, LiquidCrystal* lcd)
{
  if (!isEditable)
  {
    ShowValue(lcd, 0);
    return;
  }
  float stepp;
  switch (keycode)
  {
    case VK_RIGHT:
      stepp = calculateStep(duration, value, 1);
      if (maxVal - stepp < *value)
        *value = maxVal;
      else
        *value += stepp;

      if (callback != NULL)
        callback();
      ShowValue(lcd, 0);
      break;
    case VK_LEFT:
      stepp = calculateStep(duration, value, -1);
      if (minVal + stepp > *value)
        *value = minVal;
      else
        *value -= stepp;
      if (callback != NULL)
        callback();
      ShowValue(lcd, 0);
      break;
  }
}


/*******************TimeSelectMenuItem****************************************************************/
void TimeSelectMenuItem::showDigits(int val, LiquidCrystal* lcd)
{
  if (val < 10)
    lcd->print(0);
  lcd->print(val);
}

int TimeSelectMenuItem::calculateStep(int count, int*value, int dir)
{
  int diff = (dir == 1) ? maxVal - minVal : *value;
  if (count <= 5)
    return 1;
  if (count <= 10 || diff / 5 < 20)
  {
    (*value) -= (*value) % 5;
    return 5;
  }
  else if (count <= 15 || diff / 15 < 20)
  {
    (*value) -= (*value) % 15;
    return 15;
  }
  else if (count <= 20 || diff / 60 < 20)
  {
    (*value) -= (*value) % 60;
    return 60;
  }
  else if (count <= 25 || diff / 300 < 20)
  {
    (*value) -= (*value) % 300;
    return 300;
  }
  else if (count <= 30 || diff / 900 < 20)
  {
    (*value) -= (*value) % 900;
    return 900;
  }
  else
  {
    (*value) -= (*value) % 3600;
    return 3600;
  }
}
void TimeSelectMenuItem::ShowValue(LiquidCrystal* lcd, bool pos)
{
  if (!pos)
  {
    lcd->setCursor(1,1);
    lcd->print("   ");
    lcd->setCursor(12,1);
    lcd->print("   ");
    lcd->setCursor(4, 1);
  }
  else
    lcd->setCursor(col, row);
  int hours = *value / 3600;
  showDigits(hours, lcd);
  lcd->print(":");
  int minutes = (*value / 60) % 60;
  showDigits(minutes, lcd);
  lcd->print(":");
  int seconds = *value % 60;
  showDigits(seconds, lcd);
  if (isEditable && !pos)
  {
    lcd->setCursor(0, 1);
    if (*value > minVal)
      lcd->print("\4");
    else
      lcd->print(" ");
    lcd->setCursor(15, 1);
    if (*value < maxVal)
      lcd->print("\5");
    else
      lcd->print(" ");
  }
}

TimeSelectMenuItem::~TimeSelectMenuItem()
{
  
}
TimeSelectMenuItem::TimeSelectMenuItem(const char* label, int* value, int minVal, int maxVal, void (*callback)(), bool isEditable, byte col, byte row)
{
  this->label = label;
  this->minVal = minVal;
  this->maxVal = maxVal;
  this->value = value;
  this->callback = callback;
  this->isEditable = isEditable;
  this->col = col;
  this->row = row;
}
void TimeSelectMenuItem::ShowMenu(LiquidCrystal* lcd)
{
  lcd->setCursor(0, 0);
  lcd->print(label);
  ShowValue(lcd, 0);
}

void TimeSelectMenuItem::ShowVar(LiquidCrystal* lcd)
{
  ShowValue(lcd, 1);
}

void TimeSelectMenuItem::ProcessKey(int keycode, int duration, LiquidCrystal* lcd)
{
  if (!isEditable)
  {
    ShowValue(lcd, 0);
    return;
  }
  int stepp;
  switch (keycode)
  {
    case VK_RIGHT:
      stepp = calculateStep(duration, value, 1);
      if (maxVal - stepp < *value)
        *value = maxVal;
      else
        *value += stepp;

      if (callback != NULL)
        callback();
      ShowValue(lcd, 0);
      break;
    case VK_LEFT:
      stepp = calculateStep(duration, value, -1);
      if (minVal + stepp > *value)
        *value = minVal;
      else
        *value -= stepp;
      if (callback != NULL)
        callback();
      ShowValue(lcd, 0);
      break;
  }
}


/*******************BoolSelectMenuItem****************************************************************/
void BoolSelectMenuItem::ShowValue(LiquidCrystal* lcd, bool pos)
{
  if (!pos)
  {
    lcd->setCursor(1,1);
    lcd->print("      ");
    lcd->setCursor((16 - 1) / 2+1, 1);
    lcd->print("       ");
    
    lcd->setCursor((16 - 1) / 2, 1);
  }
  else
    lcd->setCursor(col, row);
  if (*value == 1)
    lcd->print(trueValue);
  else
    lcd->print(falseValue);

  if (isEditable && !pos)
  {
    lcd->setCursor(0, 1);
    if (*value == 1)
      lcd->print("\4");
    else
      lcd->print(" ");
    lcd->setCursor(15, 1);
    if (*value == 0)
      lcd->print("\5");
    else
      lcd->print(" ");
  }
}

BoolSelectMenuItem::~BoolSelectMenuItem() 
{
  
}

BoolSelectMenuItem::BoolSelectMenuItem(const char* label, bool* value, char trueValue, char falseValue, void (*callback)(), bool isEditable, byte col, byte row)
{
  this->label = label;
  this->trueValue = trueValue;
  this->falseValue = falseValue;
  this->value = value;
  this->callback = callback;
  this->isEditable = isEditable;
  this->col = col;
  this->row = row;
}

void BoolSelectMenuItem::ShowMenu(LiquidCrystal* lcd)
{
  lcd->setCursor(0, 0);
  lcd->print(label);
  ShowValue(lcd, 0);
}

void BoolSelectMenuItem::ShowVar(LiquidCrystal* lcd)
{
  ShowValue(lcd, 1);
}

void BoolSelectMenuItem::ProcessKey(int keycode, int duration, LiquidCrystal* lcd)
{
  if (!isEditable)
  {
    ShowValue(lcd, 0);
    return;
  }
  int stepp;
  switch (keycode)
  {
    case VK_RIGHT:
      *value = 1;
      if (callback != NULL)
        callback();
      ShowValue(lcd, 0);
      break;
    case VK_LEFT:
      *value = 0;
      if (callback != NULL)
        callback();
      ShowValue(lcd, 0);
      break;
  }
}



/*******************SaveSettingsMenuItem****************************************************************/
  
SaveSettingsMenuItem::~SaveSettingsMenuItem()
{
  
}

SaveSettingsMenuItem::SaveSettingsMenuItem()
{
}

void SaveSettingsMenuItem::ShowMenu(LiquidCrystal* lcd)
{
  lcd->setCursor(0, 0);
  lcd->print("Shrani vse      ");
  lcd->setCursor(15, 1);
  lcd->print("\5");
}

void SaveSettingsMenuItem::ProcessKey(int keycode, int duration, LiquidCrystal* lcd)
{
  switch (keycode)
  {
    case VK_RIGHT:
    case VK_LEFT:
    case VK_RETURN:
      Eprom::Store();
      lcd->setCursor(0, 1);
      lcd->print("Shranjeno");
  }
}


/*******************DefaultSettingsMenuItem****************************************************************/

DefaultSettingsMenuItem::~DefaultSettingsMenuItem()
{
  
}
DefaultSettingsMenuItem::DefaultSettingsMenuItem()
{
}

void DefaultSettingsMenuItem::ShowMenu(LiquidCrystal* lcd)
{
  lcd->setCursor(0, 0);
  lcd->print("Restore all     ");
  lcd->setCursor(0, 1);
  if (!restorePressed)
    lcd->print("Hold right key\5\5");

}

void DefaultSettingsMenuItem::ProcessKey(int keycode, int duration, LiquidCrystal* lcd)
{
  if (duration > 10)
  {
    switch (keycode)
    {
      case VK_RIGHT:
        Eprom::RestoreDefaults();
        lcd->setCursor(0, 1);
        lcd->print("Press reset     ");
        restorePressed = 1;
    }
  }
}


/*******************IntSelectMenuItem****************************************************************/

int IntSelectMenuItem::calculateStep(int count, int*value, int dir)
{
  int diff = (dir == 1) ? maxVal - minVal : *value;

  if (count <= 5 || diff < 20)
    return 1;
  if (count <= 15 || diff / 5 < 20)
  {
    (*value) -= (*value) % 5;
    return 5;
  }
  else if (count <= 20  || diff / 10 < 20)
  {
    (*value) -= (*value) % 10;
    return 10;
  }
  else if (count <= 25 || diff / 50 < 20)
  {
    (*value) -= (*value) % 50;
    return 50;
  }
  else if (count <= 30 || diff / 100 < 20)
  {
    (*value) -= (*value) % 100;
    return 100;
  }
  else if (count <= 40 || diff / 500 < 20)
  {
    (*value) -= (*value) % 500;
    return 500;
  }
  else
  {
    (*value) -= (*value) % 1000;
    return 1000;
  }
}
void IntSelectMenuItem::ShowValue(LiquidCrystal* lcd, bool pos)
{
  lcd->setCursor(col, row);
  int padding = 0;
  for (int mv = maxVal; mv != 0; mv = mv / 10)
    padding++;
  if (!pos)
  {
    lcd->setCursor(1,1);
    for(int i=1; i<(16 - padding)/2;i++)
      lcd->print(" ");
    lcd->setCursor((16 - padding) / 2 + padding, 1);
    for(int i=(16 - padding) / 2 + padding;i<15;i++)
      lcd->print(" ");
    lcd->setCursor((16 - padding) / 2, 1);
  }
  for (int mv = *value; mv != 0; mv = mv / 10)
    padding--;
  if (*value == 0)
    padding--;
  for (int i = 0; i < padding; i++)
    lcd->print(" ");
  lcd->print(*value);
  if (isEditable && !pos)
  {
    lcd->setCursor(0, 1);
    if (*value > minVal)
      lcd->print("\4");
    else
      lcd->print(" ");
    lcd->setCursor(15, 1);
    if (*value < maxVal)
      lcd->print("\5");
    else
      lcd->print(" ");
  }
}

IntSelectMenuItem::~IntSelectMenuItem()
{
  
}

IntSelectMenuItem::IntSelectMenuItem(const char* label, int* value, int minVal, int maxVal, void (*callback)(), bool isEditable, byte col, byte row)
{
  this->label = label;
  this->minVal = minVal;
  this->maxVal = maxVal;
  this->value = value;
  this->callback = callback;
  this->isEditable = isEditable;
  this->col = col;
  this->row = row;
}

void IntSelectMenuItem::ShowMenu(LiquidCrystal* lcd)
{
  lcd->setCursor(0, 0);
  lcd->print(label);
  ShowValue(lcd, 0);
}

void IntSelectMenuItem::ShowVar(LiquidCrystal* lcd)
{
  ShowValue(lcd, 1);
}

void IntSelectMenuItem::ProcessKey(int keycode, int duration, LiquidCrystal* lcd)
{
  if (!isEditable)
  {
    ShowValue(lcd, 0);
    return;
  }
  int stepp;
  switch (keycode)
  {
    case VK_RIGHT:
      stepp = calculateStep(duration, value, 1);
      if (maxVal - stepp < *value)
        *value = maxVal;
      else
        *value += stepp;

      if (callback != NULL)
        callback();
      ShowValue(lcd, 0);
      break;
    case VK_LEFT:
      stepp = calculateStep(duration, value, -1);
      if (minVal + stepp > *value)
        *value = minVal;
      else
        *value -= stepp;
      if (callback != NULL)
        callback();
      ShowValue(lcd, 0);
      break;
  }
}


