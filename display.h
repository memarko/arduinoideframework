
#include <LiquidCrystal.h>
#include <Arduino.h>
class DisplayVar
{
  public:
    virtual void ShowVar(LiquidCrystal* lcd) {};
    DisplayVar* Next = NULL;
    virtual ~DisplayVar()  {};
};

class MenuItem
{
  public:
    bool IsLocked = 0;
    virtual void ShowMenu(LiquidCrystal* lcd) {};
    MenuItem* Next = NULL;
    virtual void ProcessKey(int keycode, int duration, LiquidCrystal* lcd) {};
    virtual ~MenuItem() {};
};

class MainMenu
{
  private:
    DisplayVar*displayVar = NULL;
    MenuItem* menuItem = NULL;
    
    void (*menuSelect)(void*);
    
  protected:
    const char*line1;
    const char*line2;

    MenuItem* currItem = NULL;
    int pos = -1;
    int counter = 0;
    int totalItems = 0;

  public:
    virtual ~MainMenu();
    
    MainMenu(const char*template1, const char*template2,  void (*menuSelect)(void*));
    
    void InitTotalItems();
    
    void Show(LiquidCrystal* lcd);

    virtual void MenuSelect();

    virtual void ProcessKey(int keycode, int duration,LiquidCrystal* lcd);
    
    void AddMenuItem(MenuItem* item);
    void AddDisplayVar(DisplayVar * item);
    virtual void Update(LiquidCrystal* lcd);
};



class SubMenu: public MainMenu, public MenuItem {
  private:
    const char * line1b;
    const char * line2b;
  public:

    
    virtual ~SubMenu();
    SubMenu(const char*line1, const char*line2,  void (*menuSelect)(void*));
     
    virtual void MenuSelect();
    
    
    virtual void ShowMenu(LiquidCrystal* lcd);

    virtual void ProcessKey(int keycode, int duration, LiquidCrystal* lcd);
};

class Display: public MainMenu {
  private:
    byte lightPin;
    int currentInactiveTime = 0;
    bool screenDimmed = 0;
    bool alarm = 0;
  protected:
    LiquidCrystal* lcd;
  public:
    bool (*processHomeKeys)(int code, int duration) = NULL;
    int InactiveTime = 0;
    int Light = 100;
    void SetLight();
    void DimScreen();

    void ToogleAlarm();
    
    virtual ~Display();
    Display(LiquidCrystal* lcd, byte lightPin, byte lcdCols, byte lcdRows, const char*template1, const char*template2, void (*menuSelect)(void*));

    virtual void Update();
    virtual void ProcessKey(int keycode, int duration);
    virtual void Show();
};



class FloatSelectMenuItem : public MenuItem, public DisplayVar
{
  private:
    float modRest(float a, float b);
    float calculateStep(int count, float*value, int dir);
  protected:
    const char* label;
    float minVal;
    float maxVal;
    float delta;
    float* value;
    void (*callback)();
    bool isEditable;
    byte col;
    byte row;
    virtual void ShowValue(LiquidCrystal* lcd, bool pos);
  public:
    virtual ~FloatSelectMenuItem();
    FloatSelectMenuItem(const char* label, float* value, float minVal, float maxVal, float delta, void (*callback)(), bool isEditable, byte col, byte row);
    void ShowMenu(LiquidCrystal* lcd);
    virtual void ShowVar(LiquidCrystal* lcd);
    void ProcessKey(int keycode, int duration, LiquidCrystal* lcd);
};


class TimeSelectMenuItem : public MenuItem, public DisplayVar
{
  private:
    void showDigits(int val, LiquidCrystal* lcd);
  protected:
    const char* label;
    int minVal;
    int maxVal;
    int* value;
    void (*callback)();
    bool isEditable;
    byte col;
    byte row;
    int calculateStep(int count, int*value, int dir);
    virtual void ShowValue(LiquidCrystal* lcd, bool pos);
  public:
    virtual ~TimeSelectMenuItem();
    TimeSelectMenuItem(const char* label, int* value, int minVal, int maxVal, void (*callback)(), bool isEditable, byte col, byte row);
    void ShowMenu(LiquidCrystal* lcd);
    virtual void ShowVar(LiquidCrystal* lcd);
    void ProcessKey(int keycode, int duration, LiquidCrystal* lcd);

};




class BoolSelectMenuItem : public MenuItem, public DisplayVar
{
  protected:
    const char* label;
    char trueValue;
    char falseValue;
    bool* value;
    void (*callback)();
    bool isEditable;
    byte col;
    byte row;

    virtual void ShowValue(LiquidCrystal* lcd, bool pos);
    
  public:
    virtual ~BoolSelectMenuItem();
    BoolSelectMenuItem(const char* label, bool* value, char trueValue, char falseValue, void (*callback)(), bool isEditable, byte col, byte row);
    void ShowMenu(LiquidCrystal* lcd);
    virtual void ShowVar(LiquidCrystal* lcd);
    void ProcessKey(int keycode, int duration, LiquidCrystal* lcd);

};


class SaveSettingsMenuItem : public MenuItem
{
  public:
    virtual ~SaveSettingsMenuItem();
    SaveSettingsMenuItem();
    void ShowMenu(LiquidCrystal* lcd);

    void ProcessKey(int keycode, int duration, LiquidCrystal* lcd);

};

class DefaultSettingsMenuItem : public MenuItem
{
  private:
    bool restorePressed = 0;
  public:
    virtual ~DefaultSettingsMenuItem();
    DefaultSettingsMenuItem();
    void ShowMenu(LiquidCrystal* lcd);
    void ProcessKey(int keycode, int duration, LiquidCrystal* lcd);

};


class IntSelectMenuItem : public MenuItem, public DisplayVar
{
  private:
    int calculateStep(int count, int*value, int dir);
  protected:
    const char* label;
    int minVal;
    int maxVal;
    int* value;
    void (*callback)();
    bool isEditable;
    byte col;
    byte row;
    virtual void ShowValue(LiquidCrystal* lcd, bool pos);
  public:
    virtual ~IntSelectMenuItem();
    IntSelectMenuItem(const char* label, int* value, int minVal, int maxVal, void (*callback)(), bool isEditable, byte col, byte row);
    void ShowMenu(LiquidCrystal* lcd);

    virtual void ShowVar(LiquidCrystal* lcd);

    void ProcessKey(int keycode, int duration, LiquidCrystal* lcd);
};
