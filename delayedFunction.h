#define delayFunctionMemSize 15
void addDelayFunction(void (*funct)(), int ms);
void removeDelayFunction(void (*funct)());
void delayedFunctInit();
void execDelayFunction();
