/*

*/

#include "Arduino.h"
#include "Uln2003DirStep.hpp"
#include "PinChangeInterrupt.h"

const uint8_t phase_map[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};

// Global pointers for mapping to interrupt handlers
ULN2003 *ulnINT0; // declare a pointer to an instance for INT0 (pin2)
ULN2003 *ulnINT1; // declare a pointer to an instance for INT1 (pin3)
ULN2003 *ulnPCINT; // declare a pointer to an instance for PCINT (any input)

static void step_ISR_INT0_PIN2(void) { // global handler for INT0
  ulnINT0->step(); // calls instance handler
  int my = INT0;
}

static void step_ISR_INT1_PIN3(void) { // global handler for INT1
  ulnINT1->step(); 
}

static void step_ISR_PCINT(void) {
  ulnPCINT->step(); 
}

// Direction and enable pin interrupts are handled by PCINT.
// They are associated with the global instance pointer
// defined by the step pin interrupt (INT0, INT1, or PCINT)
static void dir_ISR_PCINT_INT0(void) {
  ulnINT0->update_dir();
}

static void dir_ISR_PCINT_INT1(void) {
  ulnINT1->update_dir();
}

static void dir_ISR_PCINT(void) {
  ulnPCINT->update_dir();
}

static void en_ISR_PCINT_INT0(void) {
  ulnINT0->update_en();
}

static void en_ISR_PCINT_INT1(void) {
  ulnINT1->update_en();
}

static void en_ISR_PCINT(void) {
  ulnPCINT->update_en();
}

// constructor overload without enable
ULN2003::ULN2003(int mot_a1, int mot_a2, int mot_b1, int mot_b2,
                 int pin_direction, int pin_step)
{
  setup(mot_a1, mot_a2, mot_b1, mot_b2, pin_direction, pin_step);
}

// constructor overload with enable
ULN2003::ULN2003(int mot_a1, int mot_a2, int mot_b1, int mot_b2,
                 int pin_direction, int pin_step, int pin_enable)
{
  setup(mot_a1, mot_a2, mot_b1, mot_b2, pin_direction, pin_step);
  setup_enable_pin(pin_enable);
}

void ULN2003::setup(int mot_a1, int mot_a2, int mot_b1, int mot_b2,
                    int pin_direction, int pin_step)
{
  // Store motor pins
  mot_pin[0] = mot_a1;
  mot_pin[1] = mot_a2;
  mot_pin[2] = mot_b1;
  mot_pin[3] = mot_b2;
  step_pin = pin_step;
  dir_pin = pin_direction;

  // Set pin modes
  for (int i=0; i<4; i++){
    pinMode(mot_pin[i], OUTPUT);
  }
  pinMode(step_pin, INPUT_PULLUP);  //pinMode(step_pin, INPUT); 
  pinMode(dir_pin, INPUT_PULLUP);

  // connect INT and PCINT interrupts
  // INT0/1 are vectored arduino external interrupts, and fast.
  // They are preferrentially used for step input pin.
  // Direction and enable are handled by lower-priority pin change
  // (PCINT) interrupts.
  if (step_pin == 2){
    ulnINT0 = this;
    attachInterrupt(digitalPinToInterrupt(step_pin), step_ISR_INT0_PIN2, RISING);
    attachPCINT(digitalPinToPCINT(dir_pin), dir_ISR_PCINT_INT0, CHANGE);
  }
  else if (step_pin == 3){
    ulnINT1 = this;
    attachInterrupt(digitalPinToInterrupt(step_pin), step_ISR_INT1_PIN3, RISING);
    attachPCINT(digitalPinToPCINT(dir_pin), dir_ISR_PCINT_INT1, CHANGE);
  }
  else {
    ulnPCINT = this;
    attachPCINT(digitalPinToPCINT(step_pin), step_ISR_PCINT, RISING);
    attachPCINT(digitalPinToPCINT(dir_pin), dir_ISR_PCINT, CHANGE);
  }
}

void ULN2003::setup_enable_pin(int pin_enable)
{
  en_pin = pin_enable;
  pinMode(en_pin, INPUT);
  if (step_pin == 2){
    //ulnINT0 = this;
    attachPCINT(digitalPinToPCINT(en_pin), en_ISR_PCINT_INT0, CHANGE);
  }
  else if (step_pin == 3){
    //ulnINT1 = this;
    attachPCINT(digitalPinToPCINT(en_pin), en_ISR_PCINT_INT1, CHANGE);
  }
  else {
    //ulnPCINT = this;
    attachPCINT(digitalPinToPCINT(en_pin), en_ISR_PCINT, CHANGE);
  }
}

void ULN2003::step(){
  phase = (phase + dir);// % 8;
  if (phase > 7)
    phase -= 8;
  if (phase < 0)
    phase += 8;

  calc_mot_val();

  digitalWrite(mot_pin[0], mot_val[0]);
  digitalWrite(mot_pin[1], mot_val[1]);
  digitalWrite(mot_pin[2], mot_val[2]);
  digitalWrite(mot_pin[3], mot_val[3]);
}

void ULN2003::update_dir(){
  if (digitalRead(dir_pin))
    dir = 1;
  else
    dir = -1;
}

void ULN2003::update_en(){
  // _EN enable pin is neg logic 
  if (digitalRead(en_pin)) {
    enabled = false;
    idle();
  }
  else {
    enabled = true;
    brake();
  }
}

void ULN2003::idle(){
  for (int i=0; i<4; i++){
    mot_val[i] = 0;
    digitalWrite(mot_pin[i], 0);
  }
}

void ULN2003::brake(){
  calc_mot_val();
  for (int i=0; i<4; i++){
    digitalWrite(mot_pin[i], mot_val[i]);
  }
}

void ULN2003::calc_mot_val(){
  if (enabled){
    mot_val[0] = ((phase_map[phase] & 0x01) == 0x01 ? true : false);
    mot_val[1] = ((phase_map[phase] & 0x02) == 0x02 ? true : false);
    mot_val[2] = ((phase_map[phase] & 0x04) == 0x04 ? true : false);
    mot_val[3] = ((phase_map[phase] & 0x08) == 0x08 ? true : false);
  } else {
    mot_val[0] = mot_val[1] = mot_val[2] = mot_val[3] = 0;
  }
}


