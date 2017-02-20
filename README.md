# Arduino IDE Framework
Helper libs for quick development. 

## How to add files?
1. Create new Arduino project in Arduino IDE.
2. Add new tab: Click arrow on the right side of tabs panel and choose Add new tab. You can also use shortcut Ctrl+Shift+N instead of searching this menu.
3. Choose one of titles bellow
4. Copy paste content of the file and name as it is in soltion
5. Repeat this for each file.

## Delayed functions
Delayed functions make multitasking environment very simple. They are very similar to Javascript setTimeout funtion. 
You do not need to worry about critical sections, context switching, complex main loops,... All you need to know is how often you need to execute the function.

1. Add following files into your project:
  - delayedFunction.h
  - delayedFunction.cpp
2. Include delayedFunction.h in your main ino project file:
  ```cpp
  #include "delayedFunction.h"
  void setup() 
  {
  
  }
  
  void loop()
  {
  // put your main code here, to run repeatedly:
  }
  ```
3. Add execDelayFunction() into the main loop. So it will execute functions:
  ```cpp
  #include "delayedFunction.h"
  void setup() 
  {
  
  }
  
  void loop()
  {
    // put your main code here, to run repeatedly:
    execDelayFunction()
  }
  ```
4. Implement your function:
  ```cpp
  #include "delayedFunction.h"
  
  // internal state of light It is initaly off 
  bool pinState = LOW; 
  
  // toggles light (switches light on or off).
  void blink()
  {
     // change internal light state from on to off or off to on
     pinState = !pinState;
     // write new internal state to the output
     digitalWrite(LED_BUILTIN, pinState); 
  }
  
  void setup() 
  {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
  }
  
  void loop()
  {
    // put your main code here, to run repeatedly:
    execDelayFunction()
  }
  ```
5. Add addDelayFunction calls: 
  ```cpp
  #include "delayedFunction.h"
  
  // internal state of light It is initially off 
  bool pinState = LOW; 
  
  // toggles light (switches light on or off).
  void blink()
  {
     // change internal light state from on to off or off to on
     pinState = !pinState;
     // write new internal state to the output
     digitalWrite(LED_BUILTIN, pinState);
     addDelayFunction(blink, 1000); //execute blink in 1000ms
  }
  
  void setup() 
  {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    addDelayFunction(blink, 1000); //execute blink in 1000ms
  }
  
  void loop()
  {
    // put your main code here, to run repeatedly:
    execDelayFunction()
  }
  ```
6. Add more functions
    ```cpp
  #include "delayedFunction.h"
  
  // internal state of light It is initially off 
  bool pinState = LOW; 
  
  // toggles light (switches light on or off).
  void blink()
  {
     // change internal light state from on to off or off to on
     pinState = !pinState;
     // write new internal state to the output
     digitalWrite(LED_BUILTIN, pinState);
     addDelayFunction(blink, 1000); //execute blink in 1000ms
  }
  
  bool pin2state = LOW;
  // toggles output of pin2 .
  void pin2toggle()
  {
     // change internal pin2state from high to low or low to high
     pin2state = !pin2state;
     // write new internal state to the output
     digitalWrite(2, pin2state);
     addDelayFunction(pin2toggle, 750); //execute blink in 750ms
  }
  
  void setup() 
  {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    addDelayFunction(blink, 1000); //execute blink in 1000ms
    addDelayFunction(pin2toggle, 750); //execute blink in 750ms
  }
  
  void loop()
  {
    // put your main code here, to run repeatedly:
    execDelayFunction()
  }
  ```
  Now led light changes state each second, but pin 2 toggles state each 750ms. 
  
  If you want function to stop executing you can use removeDelayFunction.
  
  You can execute up to 15 functions. This can be controlled in addDelayFunction.h:
  ```cpp
  #define delayFunctionMemSize 15
  ```
  You can lower this value to save memory. Each function spends 1 char + 1 int + 2 pointers = 7 bytes or more
  
  There is no addRepeatingDelayFunction yet which would make code even simpler.
  But it would occupy one more int*2 or 4 bytes) of space.
  
