/* Uln2003DirStep
 *
 * created by jeremiah heilman, July 18, 2024
 *
 * dependencies: Arduino.h, PinChangeInterrupt
 *
 * This library uses an Arduino Nano to create an interface
 * to the ULN2003 unipolar stepper driver, enabling it to be
 * controlled with "direction" and "step" inputs, similar
 * to the A4988 driver.
 *
 * The only reason I say this is for the Nano is that I've 
 * used the INT0 (pin D2) and INT1 (pin D3) interrupts for the step
 * input.  Direction and Enable pins use a lower priority PCINT vector.
 * It could be generalized to other boards with only a small effort.
 * 
 * The Nano can support up to three ULN2003's if the enable 
 * feature is not used, or two ULN2003's with dir, step, and en
 * available.
 */


#ifndef ULN2003DIRSTEP_HPP
#define ULN2003DIRSTEP_HPP

class ULN2003 {
public:
  ULN2003(int mot_a1, int mot_a2, int mot_b1, int mot_b2,
          int pin_direction, int pin_step);
  ULN2003(int mot_a1, int mot_a2, int mot_b1, int mot_b2,
          int pin_direction, int pin_step, int pin_enable);
  void step();
  void update_dir();
  void update_en();
  void idle();
  void brake();

private:
  int mot_pin[4];
  int dir_pin;
  int step_pin;
  int en_pin;
  int phase = 0;
  int dir = 1;
  bool enabled = true;
  int mot_val[4] = {0};
  void calc_mot_val();
  void setup(int mot_a1, int mot_a2, int mot_b1, int mot_b2,
             int pin_direction, int pin_step);
  void setup_enable_pin(int pin_enable);


};

#endif // ULN2003DIRSTEP_HPP