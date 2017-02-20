#include <Arduino.h>
/*********************************************************
Keyboard lib*/
#define APIN 0
#define VK_UP 38
#define VK_DOWN 40
#define VK_LEFT 37
#define VK_RIGHT 39
#define VK_RETURN 13
int previousKey = 0;
int previousPreviousKey = 0;
int countKey = 1;
int durationKey = 1;
int readKeysInt() {
  int val = analogRead(APIN);
  val = val >> 4;
  switch (val)
  {
    case 0:
      return VK_RIGHT;
    case 6:
      return VK_UP;
    case 15:
      return VK_DOWN;
    case 25:
      return VK_LEFT;
    case 39:
      return VK_RETURN;
    case 63:
    default:
      return 0;
  }
}
int readKeys(int* code, int*duration) {
  int ret = 0;
  int currentKey = readKeysInt();
  if (previousKey == currentKey && currentKey != 0)
  {
    countKey--;
    if (countKey == 0)
    {
      ret = currentKey;
      if (previousPreviousKey != currentKey)
      {
        previousPreviousKey = currentKey;
        countKey = 4;
      }
      else
      {
        countKey = 4;
        durationKey++;
      }
    }
  }
  if (currentKey == 0)
  {
    previousPreviousKey = 0;
    durationKey = 1;
  }
  previousKey = currentKey;
  if (code != NULL)
    *code = ret;
  if (duration != NULL)
  {
    *duration = durationKey;
  }
  return ret;
}
