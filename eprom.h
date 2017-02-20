#include <EEPROM.h>
#include <Arduino.h>
/*******************************
EEPROM*/
class EpromEntry {
  public:
    void* Content;
    byte Length;
    EpromEntry* Next = NULL;
};
class Eprom {
  public:
    static EpromEntry* Data;
    static void RestoreDefaults();
    static void Load();
    static void Store();
    static void Add(void*content, byte len);
    static void Add(int*content);
    static void Add(byte*content);
    static void Add(bool*content);
    static void Add(float*content);
    static void Add(double*content);
    static void Add(long*content);

    static void Add(int*content, byte len);
    static void Add(byte*content, byte len);
    static void Add(bool*content, byte len);
    static void Add(float*content, byte len);
    static void Add(double*content, byte len);
    static void Add(long*content, byte len);
};
