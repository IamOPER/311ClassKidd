// define statments should help a bit with speed
#define LED_1 2
#define LED_2 3

// Default LED states
byte LED1State = LOW;
byte LED2State = LOW;
// Default blink interval
unsigned long prevTimeLED1 = millis(), LED1BlinkDelay = 0;
unsigned long prevTimeLED2 = millis(), LED2BlinkDelay = 0;
// Variables imporant to getting data
int chosenLED = 0;
int chosenDelay = 0;



typedef void (*cycleCode)();
void GetAndSet();
void blink();

cycleCode tasks[] = {GetAndSet,blink};

int count = 0;
int task_count = sizeof(tasks)/sizeof(tasks[0]);

void setup(){
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    Serial.begin(9600);
}
// This loop runs to activate and deactiave the LED at a set rate. 
void loop(){
  tasks[count % task_count]();
  count++;
}

// The function that runs the loop to obtain the User Input to change the blink rate of LEDs
void GetAndSet(){
      if (chosenLED == 0 && chosenDelay == 0){
        Serial.print('\n');
        Serial.print("To get started LED number(1,2) and blink interval(0-n)");
        chosenLED = 1;
      }
      if (Serial.available() == 0){
        chosenLED = pickvalue("What LED? (1 or 2)");
        Serial.print("Picked LED: ");
        Serial.print(chosenLED);
        Serial.print('\n');
        // Print newline to make output look nicer
        if (chosenLED == 1 || chosenLED == 2){
        } else {
          Serial.print("LED Picked is not avaliable");
          Serial.print('\n');
          return;
        }
        Serial.print('\n');
        chosenDelay = pickvalue("What interval (in msec) for LED?");
        Serial.print("Blink Rate: ");
        Serial.print(chosenDelay);
        // Print newline to make output look nicer
        Serial.print('\n');
    }
  // Assign the correct LED only if the choice is 1 or 2
    if (chosenLED == 1){
      LED1BlinkDelay = chosenDelay;
    }
    else if (chosenLED == 2){
      LED2BlinkDelay = chosenDelay;
    }
}
// This the function that blinks the LED's at the specified declared rates. 
void blink(){
  // gets the time from the internal clock this is imporant to allow for proper intervals
    unsigned long timeNow = millis();
    // check the delay of of each LED. As long as time now will be less then the Delay time then we should be fine. 
    if (timeNow - prevTimeLED1 > LED1BlinkDelay){
        prevTimeLED1 += LED1BlinkDelay;
        // The LED switches states if the time is correct. 
        if (LED1State == HIGH)
            LED1State = LOW;
        else
            LED1State = HIGH;
      // Write out the state (turn on or off the LED);
    digitalWrite(LED_1, LED1State);
  }
    // We do the same here as the code above but for LED 2
    if (timeNow - prevTimeLED2 > LED2BlinkDelay){
        prevTimeLED2 += LED2BlinkDelay;
        if (LED2State == HIGH)
            LED2State = LOW;
        else
            LED2State = HIGH;
    digitalWrite(LED_2, LED2State);
  }
}
// The function that gets the user input to change the blink intervals of the LEDs
int pickvalue(const char* printout) {
  Serial.println(printout);
  // Wait for user input but still run the blink cycle
  while (Serial.available() == 0) {
    blink();
  }
  // Read the integer input 
  int number = Serial.parseInt();

  // Clear any remaining input for the next run
  while (Serial.available() != 0) {
    Serial.read();
  }
  return number;
}

