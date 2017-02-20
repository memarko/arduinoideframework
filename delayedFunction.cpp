/*******************************
  delayed functions
********************************/
#include "delayedFunction.h"
#include <Arduino.h>
typedef struct DelayedFunct {
  char isFree;
  int d;
  void (*funct)();
  struct DelayedFunct* next;
}
DelayedFunct;

void delayedFunctFree(DelayedFunct* funct);
DelayedFunct* delayedFunctAlloc();
void delayedFunctFree(DelayedFunct* funct);


DelayedFunct* delayFunctions = NULL;
DelayedFunct delayFunctionMem[delayFunctionMemSize];

void delayedFunctInit() {
  for (int i = 0; i < delayFunctionMemSize; i++)
  {
    delayFunctionMem[i].isFree = '\x01';
  }
}

DelayedFunct* delayedFunctAlloc() {
  for (int i = 0; i < delayFunctionMemSize; i++)
  {
    if (delayFunctionMem[i].isFree == '\x01')
    {
      delayFunctionMem[i].isFree = '\x00';
      return delayFunctionMem + i;
    }
  }
  return NULL;
}

void delayedFunctFree(DelayedFunct* funct) {
  funct->isFree = '\x01';
}


void addDelayFunction(void (*funct)(), int ms) {
  DelayedFunct** s;
  for (s = &delayFunctions; *s != NULL && (*s)->d <= ms; s = &((*s)->next));

  DelayedFunct* n = delayedFunctAlloc(); //(DelayedFunct*)malloc(sizeof(DelayedFunct));
  n->next = *s;
  n->funct = funct;
  n-> d = ms;
  *s = n;
}

void removeDelayFunction(void (*funct)()) {
  DelayedFunct** s;
  for (s = &delayFunctions; *s != NULL; s = &((*s)->next)) {
    if ((*s)->funct == funct)
    {
      DelayedFunct* d = *s;
      *s = d->next;
      delayedFunctFree(d);
    }
  }
}

void execDelayFunction() {
  if (delayFunctions != NULL)
  {
    int d = delayFunctions->d;
    if (d > 0)
      delay(d);
    int execDelay = (int)millis();
    DelayedFunct* df;
    for (df = delayFunctions; df != NULL; df = df->next)
    {
      df->d -= d;
    }
    delayFunctions->funct();
    df = delayFunctions;
    delayFunctions = df->next;
    delayedFunctFree(df);//free(df);
    execDelay = ((int)millis()) - execDelay;
    for (df = delayFunctions; df != NULL; df = df->next)
    {
      df->d -= execDelay;
    }
  }
}
