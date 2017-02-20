/*******************************
EEPROM*/
#include "eprom.h"
EpromEntry* Eprom::Data = NULL;
void Eprom::Add(void*content, byte len) {
  EpromEntry* entry = new EpromEntry();
  entry->Content = content;
  entry->Length = len;
  EpromEntry** mi;
  for (mi = &Data; *mi != NULL; mi = &((*mi)->Next));
  *mi = entry;
};
void Eprom::Add(int*content) {
  Eprom::Add((void*)content, sizeof(int));
};
void Eprom::Add(byte*content) {
  Eprom::Add((void*)content, sizeof(byte));
};
void Eprom::Add(bool*content) {
  Eprom::Add((void*)content, sizeof(bool));
};
void Eprom::Add(float*content){
     Eprom::Add((void*)content,sizeof(float));
};
void Eprom::Add(double*content) {
  Eprom::Add((void*)content, sizeof(double));
};
void Eprom::Add(long*content) {
  Eprom::Add((void*)content, sizeof(long));
};

void Eprom::Add(int*content, byte len) {
  Eprom::Add((void*)content, sizeof(int)*len);
};
void Eprom::Add(byte*content, byte len) {
  Eprom::Add((void*)content, sizeof(byte)*len);
};
void Eprom::Add(bool*content, byte len) {
  Eprom::Add((void*)content, sizeof(bool)*len);
};
void Eprom::Add(float*content, byte len){
     Eprom::Add((void*)content,sizeof(float)*len);
};
void Eprom::Add(double*content, byte len) {
  Eprom::Add((void*)content, sizeof(double)*len);
};
void Eprom::Add(long*content, byte len) {
  Eprom::Add((void*)content, sizeof(long)*len);
};

void Eprom::RestoreDefaults() {
  if (EEPROM.read(0) != 0)
    EEPROM.write(0, (byte)0);

}
void Eprom::Load() {
  int address = 1;
  if (EEPROM.read(0) != 67)
    return;
  for (EpromEntry*entry = Data; entry != NULL; entry = entry->Next)
  {
    byte* b = (byte*)entry->Content;
    for (int i = 0; i < entry->Length; i++)
    {
      b[i] = EEPROM.read(address);
      address++;
    }
  }
};
void Eprom::Store() {
  int address = 1;
  if (EEPROM.read(0) != 67)
    EEPROM.write(0, (byte)67);
  for (EpromEntry*entry = Data; entry != NULL; entry = entry->Next)
  {
    byte* b = (byte*)entry->Content;
    for (int i = 0; i < entry->Length; i++)
    {
      if (b[i] != EEPROM.read(address))
        EEPROM.write(address, b[i]);
      address++;
    }
  }
};
