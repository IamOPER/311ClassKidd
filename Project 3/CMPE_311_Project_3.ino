// --- Hardware Definitions ---
const int PWM_PIN = 9;      
const int BUTTON_PIN = 2;   

// --- PWM Constants ---
const unsigned long PWM_PERIOD_MS = 1000; // 1000ms = 1 Hz (1 Second)

// --- Logic Constants ---
const unsigned long DEBOUNCE_DELAY = 50; 

// --- Global Variables ---
unsigned long pwmOnTimeMs = 0;  // How long pin stays HIGH per cycle
unsigned long lastCycleStart = 0; // Timestamp of when the current second started

unsigned long lastDebounceTime = 0;
int buttonState = LOW;         
int lastButtonState = LOW;   
int sequenceIndex = 0;       

const float dutyCycleSequence[] = {0.0, 0.25, 0.50, 0.75, 1.0, 0.75, 0.50, 0.25};
const int SEQUENCE_LENGTH = 8;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT); 
  
  Serial.begin(9600);
  Serial.println("System Starting (Software PWM Mode - 1 Hz)...");
  
  // Start with 0% duty
  setDutyCycle(0.0);
}

void loop() {
  handleButton();
  runSoftwarePWM(); // <--- This function replaces the ISRs
}

// --- The New PWM Function ---
// This replaces the ISRs. It checks the clock and toggles the pin.
void runSoftwarePWM() {
  unsigned long currentMillis = millis();

  // 1. Check if the 1-second cycle has finished. If so, restart the timer.
  // (This replaces ISR(TIMER1_OVF_vect))
  if (currentMillis - lastCycleStart >= PWM_PERIOD_MS) {
    lastCycleStart = currentMillis;
  }

  // 2. Determine if we are in the "ON" part or "OFF" part of the cycle.
  // (This replaces the logic of ISR(TIMER1_COMPA_vect))
  unsigned long elapsedTime = currentMillis - lastCycleStart;

  if (elapsedTime < pwmOnTimeMs) {
    digitalWrite(PWM_PIN, HIGH);
  } else {
    digitalWrite(PWM_PIN, LOW);
  }
}

// --- Logic to Set the Duty Cycle ---
void setDutyCycle(float ratio) {
  // Calculate how many milliseconds the LED should be ON
  pwmOnTimeMs = (unsigned long)(PWM_PERIOD_MS * ratio);
}

// --- Button Logic (Same as before) ---
void handleButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        advanceSequence();
      }
    }
  }
  lastButtonState = reading;
}

void advanceSequence() {
  sequenceIndex++;
  if (sequenceIndex >= SEQUENCE_LENGTH) {
    sequenceIndex = 0; 
  }

  float currentRatio = dutyCycleSequence[sequenceIndex];
  
  Serial.print("Button Pressed. Duty: ");
  Serial.print(currentRatio * 100);
  Serial.println("%");

  setDutyCycle(currentRatio);
}