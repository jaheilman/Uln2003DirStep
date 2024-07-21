#include "Arduino.h"
#include "uln2003.hpp"
#include "PinChangeInterrupt.h"

#define S0_PIN1  4
#define S0_PIN2  5
#define S0_PIN3  6
#define S0_PIN4  7
#define S0_DIR   8
#define S0_EN    9  // optional
#define S0_STEP  3  // INT0 interrupt

#define S1_PIN1  A0
#define S1_PIN2  A1
#define S1_PIN3  A2
#define S1_PIN4  A3
#define S1_DIR   A4
#define S1_EN    A5 // optional
#define S1_STEP  2  // INT1 interrupt


// output pins for testing
// connect step, dir, and en to
// corresponding pins of one of the above
// steppers
#define TEST_PIN_STEP   10
#define TEST_PIN_DIR    11
#define TEST_PIN_EN     13
#define MICROS_DELAY    450    // serial to step pulse length

#define BUFF_SIZE     8
char buf[BUFF_SIZE];
char c;
int32_t k;
int32_t steps;

ULN2003 stepper0(S0_PIN1,S0_PIN2,S0_PIN3,S0_PIN4,S0_DIR,S0_STEP);
ULN2003 stepper1(S1_PIN1,S1_PIN2,S1_PIN3,S1_PIN4,S1_DIR,S1_STEP, S1_EN);

void setup() {
  Serial.begin(9600);
  Serial.print("delay (us): ");
  Serial.println(MICROS_DELAY);

  pinMode(TEST_PIN_DIR,  OUTPUT);
  pinMode(TEST_PIN_STEP, OUTPUT);
  pinMode(TEST_PIN_EN,   OUTPUT);
  digitalWrite(TEST_PIN_DIR,  1);
  digitalWrite(TEST_PIN_STEP, 1);
  digitalWrite(TEST_PIN_EN,   0);
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("ready, enter +/- steps to rotate, d = disable, e = enable");
  while (Serial.available() == 0) {}
  delay(10); // avoid orphaning string
  memset(buf, 0, BUFF_SIZE*sizeof(char));
  c = 0;
  while (Serial.available() > 0){
    buf[c++] = Serial.read();
  }
  if (buf[0] == 'd'){
    digitalWrite(TEST_PIN_EN, 1);
    //Serial.println("disabling motor");
  } else if (buf[0] == 'e') {
    digitalWrite(TEST_PIN_EN, 0);
    //Serial.println("enabling motor");
  } else { // default to stepping
    steps = atoi(buf);
    Serial.print("steps received:  ");
    Serial.println(steps);

    if (steps >= 0){
      digitalWrite(TEST_PIN_DIR, 1);
    }
    if (steps < 0){
      digitalWrite(TEST_PIN_DIR, 0);
      steps *= -1;
    }

    for (k=0; k<steps; k++){
      digitalWrite(TEST_PIN_STEP, 1);
      delayMicroseconds(MICROS_DELAY);
      digitalWrite(TEST_PIN_STEP, 0);
      delayMicroseconds(MICROS_DELAY);
    }
  }

}
