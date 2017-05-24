#include <Adafruit_NeoPixel.h>
#include <Servo.h>


#define BIG_LED 4
#define BIG_BUTTON 3
#define SERVO 2

const int BUTTON_0 = 5;
const int LED_0 = 6;

const int BUTTON_1 = 7;
const int LED_1 = 8;

const int BUTTON_2 = 9;
const int LED_2 = 10;

const int BUTTON_3 = 11;
const int LED_3 = 12;

const int NEOPIXELS_PIN = 13;
const int NUM_NEOPIXELS = 6;

int buttons[] = {BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3};
int leds[] = {LED_0, LED_1, LED_2, LED_3};

int score = 0;
int userIndexes[] = { -1, -1, -1, -1};
int lockSequence[] = {-1, -1, -1, -1};
int numPresses = 0;

Servo servo;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_NEOPIXELS, NEOPIXELS_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(BIG_LED, OUTPUT);
  pinMode(BIG_BUTTON, INPUT);
  pinMode(BUTTON_0, INPUT_PULLUP);
  pinMode(LED_0, OUTPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(LED_1, OUTPUT);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(LED_2, OUTPUT);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(LED_3, OUTPUT);
  score = 0;
  Serial.begin(9600);

  servo.attach(SERVO);
  servo.write(94); // 0 speed

  strip.begin();
  reset();
}

void loop() {
  if (digitalRead(BIG_BUTTON) == HIGH) {

    digitalWrite(BIG_LED, HIGH);
    delay(1000);
    generateLockSequence();

    // light up lock sequence
    for (int j = 0; j < 4; j++) {
      digitalWrite(leds[lockSequence[j]], HIGH);
      delay(500);
      digitalWrite(leds[lockSequence[j]], LOW);
      delay(500);
    }
    delay(500);

    boolean buttonPressed = false;
    numPresses = 0;
    while (numPresses < 4) {
      readButtons(); 
      delay(500);
    }

    boolean correct = true;
    for (int i = 0; i < 4; i++) {
      Serial.println(userIndexes[i]);
      if (userIndexes[i] != lockSequence[i]) {
        correct = false;
      }
    }
    
    if (correct) {
      Serial.print("correct");
      score = score + 1;
      updateScoreLights();

      if (score == 3) {
        delay(500);
        chase(); 
        servoEnable();
        delay(1000);
        reset();
      }

    } else {
      score = 0;
      updateScoreLights();
      reset();
    }
  }

  delay(500);

}

boolean readButtons() {
  int b0 = digitalRead(buttons[0]);
  int b1 = digitalRead(buttons[1]);
  int b2 = digitalRead(buttons[2]);
  int b3 = digitalRead(buttons[3]);
  boolean buttonPressed = false;
  int pressed = -1;
  if (b0 == LOW) {
    buttonPressed = true;
    pressed = 0;
  } else if (b1 == LOW) {
    buttonPressed = true;
    pressed = 1;

  } else if (b2 == LOW) {
    buttonPressed = true;
    pressed = 2;
  } else if (b3 == LOW) {
    buttonPressed = true;
    pressed = 3;
  }
  if (buttonPressed) {
    userIndexes[numPresses] = pressed;
    numPresses++;
  }
  return buttonPressed;
}

void reset() {
  resetUserInput();
  resetLockSequence();
  score = 0;
  updateScoreLights();
  digitalWrite(BIG_LED, LOW);
  numPresses = 0;
}

void resetUserInput() {
  for (int i = 0; i < 4; i++) {
    userIndexes[i] = -1;
  }
}

void resetLockSequence() {
  for (int i = 0; i < 4; i++) {
    lockSequence[i] = -1;
  }
}

void generateLockSequence() {
  Serial.println("sequence");
  for (int i = 0; i < 4; i++) { // sequences start at 4 length and go up
    int temp = random(4);
    Serial.println(temp);
    lockSequence[i] = temp;
  }
}

void servoEnable() {
  servo.write(180);
  delay(2000);
  servo.write(94); // stop rotation
}

void chase() {
  for(int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i  , strip.Color(255, 255, 255)); // Draw new pixel
      if (i != 0) {
        strip.setPixelColor(i-1, 0); // Erase pixel a few steps back
      }
      strip.show();
      delay(100);
  }
  strip.setPixelColor(strip.numPixels() - 1, 0); 
  strip.show();
}

void updateScoreLights() {
  if (score == 0) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
    }
  } else {
    strip.setPixelColor((score * 2) - 1, strip.Color(255, 255, 255));
    strip.setPixelColor((score * 2) - 2, strip.Color(255, 255, 255));
  }


  strip.show();
}

